#include "ProcessRunner.h"
#include "uvw.hpp"
#include "plog/Log.h"
#include "SMBlob_Node_Embedded_Windows_Console_Example.h"

ProcessRunner::ProcessRunner() {
}

void ProcessRunner::RunCommand(const std::string &command, char **argv, int argc) {
    auto loop = uvw::Loop::getDefault();

    auto processHandle = loop->resource<uvw::ProcessHandle>();

    auto pipe = loop->resource<uvw::PipeHandle>();
    processHandle->stdio(uvw::StdIN, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
    processHandle->stdio(*pipe,
                         uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());
    processHandle->stdio(uvw::StdOUT, uvw::ProcessHandle::StdIO::INHERIT_FD);

    std::stringstream ss;
    pipe->on<uvw::DataEvent>([&](const auto &evt, auto &req) {
        PLOGE_(SecondLog) << "Read RunCommand: " << command;
    });

    processHandle->on<uvw::ErrorEvent>([&](const auto &evt, auto &) {
        auto err = evt.code();
        if (err < 0) {
            PLOGE_(SecondLog) << "Error RunCommand: " << uv_err_name(err) << " str: " << uv_strerror(err);
        }
    });
    std::string result;
    processHandle->on<uvw::ExitEvent>([&result, &ss](const auto &evt, auto &req) {
        req.close();
        result = ss.str();
    });


    auto daemonArgv = std::unique_ptr<char *[]>(new char *[argc + 1]);
    if (argc > 0) {
        daemonArgv[0] = (char *) command.c_str();
        for (int i = 1; i < argc + 1; i++) {
            daemonArgv[0] = argv[i - 1];
        }
    }
    processHandle->spawn(
            (char *) command.c_str(),
            argc == 0 ? nullptr : daemonArgv.get());
    pipe->read();

    loop->run();
    loop = nullptr;
    StoreOutput(result);
}

void ProcessRunner::StoreOutput(const std::string &result) {
    outputStore = result;
}

void ProcessRunner::DisplayOutput() {
    std::cout << outputStore << std::endl;
}

ProcessRunner::~ProcessRunner() {
}

const std::string &ProcessRunner::getOutputStore() const {
    return outputStore;
}
