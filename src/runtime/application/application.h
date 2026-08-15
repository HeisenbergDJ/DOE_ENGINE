/********************************************************************
 * "application.h" declares the runtime Application class.         *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_APPLICATION_APPLICATION_H_
#define DOE_ENGINE_RUNTIME_APPLICATION_APPLICATION_H_

#include "application_config.h"
#include "scalar_value.h"
#include "tagger.h"

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace doe::runtime {

// A runtime Application is instantiated from an ApplicationConfig. The
// constructor is the only place that reads the Config: it converts the command
// spec and input/output lists into the members the execution needs and compiles
// each tagged_copy input into a ready Tagger. Run() then executes a prepared
// Application for one design point, taking the variable values that feed the
// taggers during tagged_copy input preparation: prepare inputs (copy or
// tagged_copy), launch the external program, check exit codes and required
// outputs, and return the logical output port to physical file path mapping.
class Application {
public:
    explicit Application(doe::config::ApplicationConfig config);

    std::map<std::string, std::filesystem::path> Run(
        const std::map<std::string, ScalarValue>& variable_values) const;

private:
    struct Input {
        doe::config::ApplicationInputMode mode = doe::config::ApplicationInputMode::kCopy;
        std::string source;
        std::string destination;
        std::unique_ptr<Tagger> tagger;
    };

    struct Output {
        std::string port;
        doe::config::ArtifactType type = doe::config::ArtifactType::kFile;
        std::string path;
        bool required = true;
    };

    std::string ExpandTemplate(const std::string& text) const;
    std::filesystem::path ExpandPath(const std::string& text) const;

    void PrepareInputs(const std::map<std::string, ScalarValue>& variable_values) const;
    void PrepareCopy(const Input& input) const;
    void PrepareTaggedCopy(const Input& input,
                           const std::map<std::string, ScalarValue>& variable_values) const;
    void Execute() const;
    void CheckOutputs(std::map<std::string, std::filesystem::path>& output_ports) const;

    std::string executable_;
    std::string working_directory_;
    std::vector<std::string> arguments_;
    long long timeout_seconds_ = 0;
    std::vector<int> success_exit_codes_;
    std::vector<Input> inputs_;
    std::vector<Output> outputs_;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_APPLICATION_APPLICATION_H_
