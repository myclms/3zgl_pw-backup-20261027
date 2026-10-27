# Issue #1469 验证记录

修复在共享层逐段引用列名，将名字内部的反引号加倍。普通列名、ENUM/SET 的 unsigned CAST、普通别名和 `__SHADOW_` 别名使用同一引用方式，不引入对 SQL 模块的依赖。

- `issue-1469-reproduction.json`：官方未修复 Windows seekdb 1.4.0.0 上的实际 SQL 复现。5 个特殊列名索引创建均返回 1064；普通列名对照成功。18 条普通列名对照 SQL 的结果通过断言。
- `fix-1469.json`：实际旧、新生产函数在最小类型适配器中的编译与验证结果。8,512 个格式化场景、268,532 项断言通过；5,472 个反引号场景与旧函数输出不同。
- `ddl_formatter_harness.cpp`：上述聚焦验证使用的源码，包含从官方基线和修复中提取的函数体。使用的适配器不代表完整 OBLib 或数据库集成。
- `fix-1469.patch`：相对官方基线 `4b28bcbff0a476b25f2f668c2560a1b77b2095e5` 的修复与 mysqltest 回归。
- `PR_DESCRIPTION.md`：PR 正文草稿，明确记录已运行和待验证项目。

本地编译命令：

```powershell
.work/zig-toolchain/ziglang/zig.exe c++ -std=c++17 -Wall -Wextra -Werror -fsanitize=undefined -fsanitize-trap=undefined validation/ddl_formatter_harness.cpp -o .work/ddl_formatter_harness.exe
.work/ddl_formatter_harness.exe
```

新增 `ddl.ddl_identifier_escaping` mysqltest 覆盖反引号主键、ENUM/SET 列、连续反引号、Unicode、普通列名、NULL 数据和唯一索引元数据。已登记到旧版测试列表。

完整修复后的 seekdb 和实际 mysqltest 可执行程序尚未在本机编译、运行；不将聚焦函数验证或未修复引擎的对照结果视为完整集成测试通过。
