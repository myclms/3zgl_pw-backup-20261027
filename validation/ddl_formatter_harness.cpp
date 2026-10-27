
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
constexpr int OB_SUCCESS = 0;
constexpr int OB_INVALID_ARGUMENT = -4002;
constexpr int FAIL_APPEND = -4013;
#define OB_FAIL(x) ((ret = (x)) != OB_SUCCESS)
#define OB_SUCC(x) ((x) == OB_SUCCESS)
#define OB_UNLIKELY(x) (x)
#define OB_ISNULL(x) ((x) == nullptr)
struct ObString {
  const char *data;
  int64_t size;
  const char *ptr() const { return data; }
  int32_t length() const { return static_cast<int32_t>(size); }
};
struct ObColumnNameInfo {
  ObString column_name_;
  bool is_shadow_column_;
  bool is_enum_set_need_cast_;
};
struct ObSqlString {
  std::string data;
  int calls = 0;
  int fail_at = 0;
  int calls_after_failure = 0;
  bool failed = false;
  int append(const char *s, int64_t n) {
    if (failed) ++calls_after_failure;
    if (++calls == fail_at) { failed = true; return FAIL_APPEND; }
    if (n < 0 || (n > 0 && s == nullptr)) return OB_INVALID_ARGUMENT;
    if (n) data.append(s, static_cast<size_t>(n));
    return OB_SUCCESS;
  }
  int append(const char *s) { return append(s, std::strlen(s)); }
  int append_fmt(const char *fmt, ...) {
    char buf[8192];
    va_list ap;
    va_start(ap, fmt);
    const int len = std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (len < 0 || len >= static_cast<int>(sizeof(buf))) throw std::runtime_error("format");
    return append(buf, len);
  }
};
struct ObDDLUtil {
  static int generate_column_name_str(const ObColumnNameInfo &, bool, bool, bool, ObSqlString &);
};
struct OldDDLUtil {
  static int generate_column_name_str(const ObColumnNameInfo &, bool, bool, bool, ObSqlString &);
};

int OldDDLUtil::generate_column_name_str(
    const ObColumnNameInfo &column_name_info,
    const bool with_origin_name,
    const bool with_alias_name,
    const bool with_comma,
    ObSqlString &sql_string)
{
  int ret = OB_SUCCESS;
  const char *split_char = "`";
  // append comma
  if (with_comma) {
    if (OB_FAIL(sql_string.append_fmt(", "))) {
    }
  }
  // append original column name
  if (OB_SUCC(ret) && with_origin_name) {
    if (column_name_info.is_enum_set_need_cast_) {
      // Enum and set values are cast to unsigned before being appended to a macro block.
      if (OB_FAIL(sql_string.append_fmt("cast(%s%.*s%s as unsigned)", split_char, column_name_info.column_name_.length(), column_name_info.column_name_.ptr(), split_char))) {
      }
    } else if (OB_FAIL(sql_string.append_fmt("%s%.*s%s", split_char, column_name_info.column_name_.length(), column_name_info.column_name_.ptr(), split_char))) {
    }
  }
  // append AS
  if (OB_SUCC(ret) && with_origin_name && with_alias_name) {
    if (OB_FAIL(sql_string.append_fmt(" AS "))) {
    }
  }
  // append alias column name
  if (OB_SUCC(ret) && with_alias_name) {
    if (OB_FAIL(sql_string.append_fmt("%s%s%.*s%s", split_char, column_name_info.is_shadow_column_ ? "__SHADOW_" : "",
        column_name_info.column_name_.length(), column_name_info.column_name_.ptr(), split_char))) {
    }
  }
  return ret;
}

static int append_ddl_column_identifier(
    const ObString &column_name, const bool is_shadow, ObSqlString &sql_string)
{
  int ret = OB_SUCCESS;
  const int64_t name_len = column_name.length();
  if (OB_UNLIKELY(name_len < 0 || (name_len > 0 && OB_ISNULL(column_name.ptr())))) {
    ret = OB_INVALID_ARGUMENT;
  } else if (OB_FAIL(sql_string.append("`"))) {
  } else if (is_shadow && OB_FAIL(sql_string.append("__SHADOW_"))) {
  } else {
    int64_t start = 0;
    for (int64_t i = 0; OB_SUCC(ret) && i < name_len; ++i) {
      if ('`' == column_name.ptr()[i]) {
        // Append through the backtick, then double it inside the quoted name.
        if (OB_FAIL(sql_string.append(column_name.ptr() + start, i - start + 1))) {
        } else if (OB_FAIL(sql_string.append("`"))) {
        }
        start = i + 1;
      }
    }
    if (OB_SUCC(ret) && start < name_len) {
      ret = sql_string.append(column_name.ptr() + start, name_len - start);
    }
    if (OB_SUCC(ret)) {
      ret = sql_string.append("`");
    }
  }
  return ret;
}

int ObDDLUtil::generate_column_name_str(
    const ObColumnNameInfo &column_name_info,
    const bool with_origin_name,
    const bool with_alias_name,
    const bool with_comma,
    ObSqlString &sql_string)
{
  int ret = OB_SUCCESS;
  // append comma
  if (with_comma) {
    if (OB_FAIL(sql_string.append_fmt(", "))) {
    }
  }
  // append original column name
  if (OB_SUCC(ret) && with_origin_name) {
    // Enum and set values are cast to unsigned before being appended to a macro block.
    if (column_name_info.is_enum_set_need_cast_ && OB_FAIL(sql_string.append("cast("))) {
    } else if (OB_FAIL(append_ddl_column_identifier(column_name_info.column_name_, false, sql_string))) {
    } else if (column_name_info.is_enum_set_need_cast_ && OB_FAIL(sql_string.append(" as unsigned)"))) {
    }
  }
  // append AS
  if (OB_SUCC(ret) && with_origin_name && with_alias_name) {
    if (OB_FAIL(sql_string.append_fmt(" AS "))) {
    }
  }
  // append alias column name
  if (OB_SUCC(ret) && with_alias_name) {
    if (OB_FAIL(append_ddl_column_identifier(column_name_info.column_name_, column_name_info.is_shadow_column_, sql_string))) {
    }
  }
  return ret;
}


int checks = 0;
void check(bool ok, const std::string &label) {
  ++checks;
  if (!ok) throw std::runtime_error(label);
}
std::string parse_identifier(const std::string &text, size_t &pos) {
  check(pos < text.size() && text[pos++] == '`', "opening quote");
  std::string value;
  while (pos < text.size()) {
    const char c = text[pos++];
    if (c != '`') { value += c; continue; }
    if (pos < text.size() && text[pos] == '`') { ++pos; value += '`'; }
    else return value;
  }
  throw std::runtime_error("unterminated identifier");
}
void token(const std::string &s, size_t &pos, const std::string &expected) {
  check(s.compare(pos, expected.size(), expected) == 0, "expected " + expected);
  pos += expected.size();
}
int main() {
  try {
    std::vector<std::string> names = {
      "ordinary", "select", "a`b", "`leading", "trailing`", "a``b", "`", "",
      "\xe5\x88\x97`\xe5\x90\x8d", std::string("a\0`b", 4)
    };
    // A deterministic corpus checks round-trip names rather than reimplementing escaping.
    const char alphabet[] = "ab` _012";
    uint32_t rng = 1469;
    for (int i = 0; i < 256; ++i) {
      std::string name;
      for (int j = 0; j < i % 63; ++j) {
        rng = rng * 1664525u + 1013904223u;
        name += alphabet[(rng >> 16) % (sizeof(alphabet) - 1)];
      }
      names.push_back(name);
    }
    int previous_failures = 0;
    int cases = 0;
    for (const std::string &name : names) {
      for (int bits = 0; bits < 32; ++bits) {
        const bool origin = bits & 1, alias = bits & 2, comma = bits & 4;
        const bool shadow = bits & 8, cast = bits & 16;
        ObColumnNameInfo info{{name.data(), static_cast<int64_t>(name.size())}, shadow, cast};
        ObSqlString out;
        out.data = "prefix";
        check(ObDDLUtil::generate_column_name_str(info, origin, alias, comma, out) == 0, "format success");
        size_t pos = 0;
        token(out.data, pos, "prefix");
        if (comma) token(out.data, pos, ", ");
        if (origin) {
          if (cast) token(out.data, pos, "cast(");
          check(parse_identifier(out.data, pos) == name, "original round trip");
          if (cast) token(out.data, pos, " as unsigned)");
        }
        if (origin && alias) token(out.data, pos, " AS ");
        if (alias) check(parse_identifier(out.data, pos) == (shadow ? "__SHADOW_" : "") + name, "alias round trip");
        check(pos == out.data.size(), "no trailing SQL");
        // Every individual append failure must stop output and preserve the error.
        for (int failure = 1; failure <= out.calls; ++failure) {
          ObSqlString failing;
          failing.data = "prefix";
          failing.fail_at = failure;
          check(ObDDLUtil::generate_column_name_str(info, origin, alias, comma, failing) == FAIL_APPEND, "error propagated");
          check(failing.calls == failure && failing.calls_after_failure == 0, "stop after append failure");
        }
        if (name.find('\0') == std::string::npos) {
          ObSqlString before;
          before.data = "prefix";
          check(OldDDLUtil::generate_column_name_str(info, origin, alias, comma, before) == 0, "old success");
          if (name.find('`') == std::string::npos) check(before.data == out.data, "unchanged ordinary names");
          else if (origin || alias) { check(before.data != out.data, "backtick regression differs before fix"); ++previous_failures; }
        }
        ++cases;
      }
    }
    ObSqlString truncated;
    const char raw[] = {'a', '`', 'b', 'Z'};
    ObColumnNameInfo info{{raw, 3}, false, false};
    check(ObDDLUtil::generate_column_name_str(info, true, false, false, truncated) == 0, "length-limited input");
    check(truncated.data == "`a``b`", "respects input length");
    ObSqlString invalid;
    check(append_ddl_column_identifier({nullptr, 1}, false, invalid) == OB_INVALID_ARGUMENT, "invalid input");
    check(invalid.calls == 0, "reject before appending");
    std::cout << "PASS: " << cases << " formatter cases, " << checks << " assertions; "
              << previous_failures << " backtick cases differ from the old formatter.\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "FAIL: " << e.what() << '\n';
    return 1;
  }
}
