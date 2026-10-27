# 参赛提交记录

赛题入口：https://open.oceanbase.com/train/detail/21?questionId=600071

## 部署

本机原生运行包来自 OceanBase 官方 `seekdb-go` 仓库，固定提交为 `7c4510e6b47849ca7c26fb67c979d4c4ab5e5db7`。运行包 SHA-256：

`3a50225198ac67f26f1777dd02cd6a5357ceb0a7ed33fa78db05a2e7a33af295`

实际验证结果：

```text
version: 5.7.25-OceanBase seekdb-v1.4.0.0
SELECT 1 + 1: 2
content.id: 5180782a-af0c-8662-aaf5-de5101f0e12b
telemetry sent: true
memory_budget: 1G
log_disk_size: 2G
```

提测 UUID 填写：`5180782a-af0c-8662-aaf5-de5101f0e12b`。

UUID 来自 `local-seekdb/instance/run/telemetry.json` 的 `content.id`。不要提交整个遥测文件。`sent: true` 表示本地发送状态，最终认领和得分仍以比赛平台核验结果为准。

已有 AMD 云端部署材料保存在 `ps/deployment.png`，旧 UUID 为 `f4e9af1d-0ee1-865d-a873-2282785160e9`。若本队已用旧 UUID 通过部署项，请保留原有得分记录，无需为同一子题重复认领。

## Fork 与 PR

原有真实 Fork：https://github.com/myclms/seekdb

报名仓库必须保留为：https://github.com/myclms/3zgl_pw

原 `3zgl_pw` 是独立仓库，尚未加入官方 Fork 网络。已获授权保留报名地址：备份原仓库，再将真正的官方 Fork 改名为 `3zgl_pw`，并保留现有提交与材料。改名完成后应在“Fork 仓库地址”中填写新的完整 HTTPS 地址。

修复分支：`codex/fix-ddl-identifier-escaping`。

修复提交：`1bddd3c03e1a9f982930d760d15fd33bf82f66dd`。

关联 Issue：https://github.com/oceanbase/seekdb/issues/1469

正式 PR 尚未创建。仓库改名完成后，从上述报名仓库的修复分支向 `oceanbase/seekdb:master` 发起 PR，并填写比赛平台要求的官方 PR 完整地址。

PR 草稿和测试边界见 `validation/PR_DESCRIPTION.md`。完整修复后的数据库编译和 mysqltest 运行仍待 CI 验证。
