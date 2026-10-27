# 3zgl_pw

2026 OceanBase 数据库大赛基础赛题的部署、贡献和验证记录。

- 本机 seekdb 1.4.0.0 已启动并完成 SQL 验证，部署 UUID：`5180782a-af0c-8662-aaf5-de5101f0e12b`。
- 已完成官方仓库的真实 Fork：[myclms/3zgl_pw](https://github.com/myclms/3zgl_pw)。
- 已提交大赛 [Issue #1469](https://github.com/oceanbase/seekdb/issues/1469) 的修复 PR：[oceanbase/seekdb#1897](https://github.com/oceanbase/seekdb/pull/1897)。来源仓库已核验与报名地址一致。

原仓库的提交和材料完整保存在 [3zgl_pw-backup-20261027](https://github.com/myclms/3zgl_pw-backup-20261027)，并同步到报名仓库的 `codex/competition-evidence` 分支。

详细信息见 [提交说明](SUBMISSION.md) 和 [验证记录](validation/README.md)。

本地数据库通过 `local-seekdb/endpoint.json` 提供连接信息；端口由运行时分配，仅监听本机回环地址。数据、日志和运行包保存在本项目内，不提交到 Git。

```powershell
python scripts/local_seekdb.py verify
python scripts/local_seekdb.py stop
python scripts/local_seekdb.py serve
```

`serve` 会保持运行，关闭 SQL 客户端后可使用另一个终端中的 `stop` 命令正常停止实例。
