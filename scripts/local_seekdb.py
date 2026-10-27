"""Run the checksum-verified official Windows seekdb runtime in this project."""
import argparse
import ctypes
import json
import os
from pathlib import Path
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
STATE = ROOT / 'local-seekdb'
DATABASE = STATE / 'instance'
RUNTIME = ROOT / '.work' / 'runtime'


class ConnectionOptions(ctypes.Structure):
    _fields_ = [('transport', ctypes.c_char_p), ('port', ctypes.c_uint),
                ('host', ctypes.c_char_p), ('unix_socket', ctypes.c_char_p),
                ('named_pipe', ctypes.c_char_p), ('user', ctypes.c_char_p)]


def serve():
    STATE.mkdir(exist_ok=True)
    stop_file = STATE / 'stop'
    if stop_file.exists():
        stop_file.unlink()
    # The managed child executable must find the runtime DLLs as well.
    os.environ['PATH'] = str(RUNTIME) + os.pathsep + os.environ.get('PATH', '')
    dll_directory = os.add_dll_directory(str(RUNTIME))
    lib = ctypes.CDLL(str(RUNTIME / 'seekdb.dll'))
    lib.seekdb_open.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p),
                               ctypes.POINTER(ctypes.c_void_p)]
    lib.seekdb_open.restype = ctypes.c_int
    lib.seekdb_last_open_error.restype = ctypes.c_char_p
    lib.seekdb_connection_options.argtypes = [ctypes.c_void_p, ctypes.POINTER(ConnectionOptions)]
    lib.seekdb_close.argtypes = [ctypes.c_void_p]
    args = [b'memory_budget', b'1G', b'log_disk_size', b'2G',
            b'datafile_size', b'512M', b'datafile_maxsize', b'2G']
    parameters = (ctypes.c_char_p * (len(args) + 1))(*args, None)
    handle = ctypes.c_void_p()
    rc = lib.seekdb_open(str(DATABASE).encode('utf-8'), parameters, ctypes.byref(handle))
    if rc != 0:
        raise RuntimeError('seekdb_open: ' + str(lib.seekdb_last_open_error()))
    try:
        options = ConnectionOptions()
        rc = lib.seekdb_connection_options(handle, ctypes.byref(options))
        if rc != 0:
            raise RuntimeError('Cannot obtain seekdb connection options: ' + str(rc))
        endpoint = {key: getattr(options, key).decode('utf-8') if getattr(options, key) else None
                    for key in ['transport', 'host', 'unix_socket', 'named_pipe', 'user']}
        endpoint.update(port=options.port, controller_pid=os.getpid())
        if endpoint['transport'] != 'tcp' or endpoint['host'] not in ['127.0.0.1', '::1', 'localhost']:
            raise RuntimeError('Expected a loopback TCP endpoint: ' + str(endpoint))
        (STATE / 'endpoint.json').write_text(json.dumps(endpoint, indent=2), encoding='utf-8')
        print('READY ' + json.dumps(endpoint), flush=True)
        while not stop_file.exists():
            time.sleep(1)
    finally:
        lib.seekdb_close(handle)
        dll_directory.close()
        print('STOPPED', flush=True)


def verify():
    sys.path.insert(0, str(ROOT / '.work' / 'python-deps'))
    import pymysql
    endpoint = json.loads((STATE / 'endpoint.json').read_text(encoding='utf-8'))
    with pymysql.connect(host=endpoint['host'], port=endpoint['port'],
                         user=endpoint['user'], database='test', autocommit=True) as conn:
        with conn.cursor() as cursor:
            cursor.execute('SELECT VERSION(), 1 + 1')
            version, value = cursor.fetchone()
            assert value == 2
    telemetry_path = DATABASE / 'run' / 'telemetry.json'
    telemetry = json.loads(telemetry_path.read_text(encoding='utf-8'))
    result = {'version': version, 'sql_1_plus_1': value,
              'deployment_uuid': telemetry['content']['id'],
              'telemetry_file': str(telemetry_path)}
    (STATE / 'verification.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=['serve', 'verify', 'stop'])
    action = parser.parse_args().action
    if action == 'serve':
        serve()
    elif action == 'verify':
        verify()
    else:
        (STATE / 'stop').touch()
