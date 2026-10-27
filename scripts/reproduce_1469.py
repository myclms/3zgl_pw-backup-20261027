"""Reproduce #1469 on the unpatched official local binary and check fixture controls."""
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / '.work/python-deps'))
import pymysql

endpoint = json.loads((ROOT / 'local-seekdb/endpoint.json').read_text(encoding='utf-8'))
case = ROOT / '.work/seekdb-git/tools/deploy/mysql_test/test_suite/ddl/t/ddl_identifier_escaping.test'
text = '\n'.join(line for line in case.read_text(encoding='utf-8').splitlines()
                 if not line.startswith(('#', '--')))
statements = [sql.strip() for sql in text.split(';') if sql.strip()]
report = {'binary': 'official unpatched seekdb 1.4.0.0', 'reproduction': [], 'control_selects': []}
conn = pymysql.connect(host=endpoint['host'], port=endpoint['port'], user=endpoint['user'],
                       database='test', charset='utf8mb4', autocommit=True,
                       read_timeout=30, write_timeout=30)
with conn.cursor() as cursor:
    cursor.execute("ALTER SYSTEM SET memory_budget='1G'")
    cursor.execute("SHOW PARAMETERS LIKE 'memory_budget'")
    report['memory_budget'] = cursor.fetchone()[3]
    for sql in statements:
        try:
            print('RUN ' + sql, flush=True)
            cursor.execute(sql)
            rows = cursor.fetchall() if cursor.description else []
            if sql.startswith('CREATE UNIQUE INDEX'):
                report['reproduction'].append({'sql': sql, 'success': True})
                assert 'uk_plain_control' in sql, 'Expected quoted-identifier bug was not reproduced'
        except pymysql.MySQLError as error:
            if not sql.startswith('CREATE UNIQUE INDEX') or 'uk_plain_control' in sql:
                raise
            report['reproduction'].append({'sql': sql, 'success': False,
                                           'error_code': error.args[0], 'error': error.args[1]})
            assert error.args[0] == 1064, error
    # Execute the same fixture with ordinary identifiers to validate its non-bug paths.
    replacements = {'ddl_identifier_escaping': 'ddl_identifier_control', '`p``k`': '`pk`',
                    '`e``n`': '`en`', '`s``t`': '`st`', '`a````b`': '`ab`', '`列``名`': '`列名`'}
    for sql in statements:
        for old, new in replacements.items():
            sql = sql.replace(old, new)
        cursor.execute(sql)
        if cursor.description:
            rows = cursor.fetchall()
            report['control_selects'].append({'sql': sql, 'rows': rows})
    report['control_statement_count'] = len(statements)
    expected = [[(1, 'active'), (2, 'paused')], [(1, 'first', 'red', 'x', 'x', 'active'),
                         (2, 'second', 'blue', 'y', 'y', 'paused'),
                         (3, None, None, None, None, None), (4, None, None, None, None, None)],
                [(5,)]]
    assert [list(x['rows']) for x in report['control_selects']] == expected
conn.close()
(ROOT / 'validation/issue-1469-reproduction.json').write_text(
    json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False, indent=2))
