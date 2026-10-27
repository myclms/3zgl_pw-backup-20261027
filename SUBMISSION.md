# 参赛提交记录

赛题入口：https://open.oceanbase.com/train/detail/21?questionId=600071

**已于 2026-10-27 14:37（北京时间）提交并通过三项核验：30/30，成功 3 项、失败 0 项。**

成绩记录：https://open.oceanbase.com/train/TestRecord?questionId=600071

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

提测“Fork 仓库地址”：https://github.com/myclms/3zgl_pw

报名仓库必须保留为：https://github.com/myclms/3zgl_pw

已按授权完成仓库改名：原独立仓库完整备份到 https://github.com/myclms/3zgl_pw-backup-20261027 ，真正的官方 Fork 使用 `myclms/3zgl_pw`。报名地址保持原值。原仓库稳定 ID 为 `1406772094`，现报名 Fork 稳定 ID 为 `1434365923`，父仓库确认为 `oceanbase/seekdb`。

修复分支：`codex/fix-ddl-identifier-escaping`。

修复提交：`1bddd3c03e1a9f982930d760d15fd33bf82f66dd`。

关联 Issue：https://github.com/oceanbase/seekdb/issues/1469

提测“PR 链接”：https://github.com/oceanbase/seekdb/pull/1897

PR 已创建，状态为 open，目标为 `oceanbase/seekdb:master`，来源为 `myclms/3zgl_pw:codex/fix-ddl-identifier-escaping`。GitHub 确认无合并冲突；尚未合并。赛题按已提交核验，不要求等待合并。

PR 正文和测试边界见 `validation/PR_DESCRIPTION.md`。完整修复后的数据库编译和 mysqltest 运行仍待 CI 验证。本基础实践题的平台核验已完成，最新成绩和最高成绩均为 30.000。

## 本队验证行与平台核验

提测页的最新说明要求先获取“本队验证行”，然后原样、独立一行分别放入公开 Fork 默认分支的 README 和官方 PR 的正文。此说明已允许公开实践材料与私有报名仓库分离，不再要求 PR 来自报名仓库；本次提交仍保留了相同的仓库地址。

本队验证行已由用户提供，并与登录后的提测表单核对一致：

```text
OB-P:Bcl1kH2tJcwXqfpfmeKkjA
```

已确认 Fork 默认分支 `master` 的 README 中存在这条独立验证行；官方 PR #1897 正文也已补入相同独立验证行。随后提交了本机部署 UUID、Fork 地址及 PR 链接，平台记录显示“执行成功”，30.000 分，成功 3 项、失败 0 项。

成绩截图保存在本机 `validation/platform-result.png`，其中包含历史提测记录，因此未发布到公开仓库。

PR 自动标签和翻译检查已通过。完整 CI 等待维护者批准运行；`license/cla` 仍等待贡献者本人阅读并签署，入口：https://cla-assistant.io/oceanbase/seekdb?pullRequest=1897 。
