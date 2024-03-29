#pragma once

#define __CURRENT_PID__  plog::util::gettid()

#define __SEVERITY__(logSeverity, defValue) logSeverity >= static_cast<int>(plog::Severity::none) && logSeverity <= static_cast<int>(plog::Severity::verbose) ?  static_cast<plog::Severity>(logSeverity) : defValue

#define __SEVERITY_D__(logSeverity) __SEVERITY__(logSeverity, plog::Severity::debug)

#define SMBEW_S_PIPE_NAME std::string("--pipe-name")
#define SMBEW_S_LOG std::string("--log")
#define SMBEW_S_LOG_FILE std::string("--logFile")
#define SMBEW_S_DEBUG std::string("--debug")