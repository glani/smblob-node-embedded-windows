#include "ProcessRunner.h"
#include "uvw.hpp"
#include "plog/Log.h"
#include "SMBlob_Node_Embedded_Windows_Console_Example.h"

ProcessRunner::ProcessRunner() : status(-1) {
}

ProcessRunner::~ProcessRunner() {
}

void ProcessRunner::RunCommand(const std::string &command, char **argv, int argc) {
    auto loop = uvw::Loop::getDefault();

//    uvw::ProcessHandle::disableStdIOInheritance();
    auto processHandle = loop->resource<uvw::ProcessHandle>();

    auto pipeStdout = loop->resource<uvw::PipeHandle>();
    auto pipeStderr = loop->resource<uvw::PipeHandle>();
    processHandle->stdio(uvw::StdIN, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
    processHandle->stdio(*pipeStdout,
                         uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());
    processHandle->stdio(*pipeStderr,
                         uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());

//    processHandle->stdio(uvw::StdERR, uvw::ProcessHandle::StdIO::INHERIT_FD);
//    processHandle->stdio(uvw::StdOUT, uvw::ProcessHandle::StdIO::INHERIT_FD);

    std::stringstream ssStdOut;
    pipeStdout->on<uvw::DataEvent>([&](const auto &evt, auto &req) {
        PLOGE_(SecondLog) << "pipeStdout Read RunCommand: " << command;
        if (evt.data) {
            ssStdOut << evt.data.get();
        }
    });
    std::stringstream ssStdErr;
    pipeStderr->on<uvw::DataEvent>([&](const auto &evt, auto &req) {
        PLOGE_(SecondLog) << "pipeStderr Read RunCommand: " << command;
        if (evt.data) {
            ssStdErr << evt.data.get();
        }
    });

    processHandle->on<uvw::ErrorEvent>([&](const auto &evt, auto &) {
        auto err = evt.code();
        if (err < 0) {
            PLOGE_(SecondLog) << "Error RunCommand: " << uv_err_name(err) << " str: " << uv_strerror(err);
        }
    });
    processHandle->on<uvw::ExitEvent>([&](const uvw::ExitEvent &evt, auto &req) {
        StoreStatus(evt.status);
        req.close();
    });


    auto daemonArgv = std::unique_ptr<char *[]>(new char *[argc + 1]);
    if (argc > 0) {
        daemonArgv[0] = (char *) command.c_str();
        for (int i = 1; i < argc + 1; i++) {
            daemonArgv[i] = argv[i - 1];
        }
    }
    processHandle->spawn(
            (char *) command.c_str(),
            argc == 0 ? nullptr : daemonArgv.get());
    pipeStdout->read();
    pipeStderr->read();

    loop->run();
    loop = nullptr;
    StoreOutput(ssStdOut.str());
    StoreError(ssStdErr.str());
}

int64_t ProcessRunner::getStatus() const {
    return status;
}

void ProcessRunner::StoreOutput(const std::string &result) {
    output = result;
}

void ProcessRunner::DisplayOutput() {
    std::cout << output << std::endl;
}

void ProcessRunner::StoreStatus(int64_t status) {
    this->status = status;
}

void ProcessRunner::StoreError(const std::string &result) {
    error = result;
}

const std::string &ProcessRunner::getOutput() const {
    return output;
}

const std::string &ProcessRunner::getError() const {
    return error;
}

