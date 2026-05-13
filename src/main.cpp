#include "OutputWriter.h"
#include "Parser.h"
#include "Preprocessor.h"
#include "Scheduler.h"
#include <fstream>
#include <iostream>

struct Arguments {
    std::string coursesFile;
    std::string periodsFile;
    std::string selectedProgramsFile;
    std::string outputFile;
    int perBlockLimit = -1;
    int combinedLimit = -1;
    double maxRuntimeSeconds = 30.0;
};

void printUsage() {
    std::cerr
        << "Usage:\n"
        << "  exam_scheduler"
        << " --courses <courses.txt>"
        << " --periods <periods.txt>"
        << " --selected-programs <selected_programs.txt>"
        << " --output <output.txt>"
        << " [--per-block-limit N]"
        << " [--combined-limit N]"
        << " [--max-runtime-seconds N]\n";
}

Arguments parseArguments(int argc, char* argv[]) {
    Arguments args;

    for (int i = 1; i < argc; ++i) {
        std::string key = argv[i];
        if (i + 1 >= argc) {
            throw std::runtime_error("Missing value for argument: " + key);
        }

        std::string value = argv[++i];
        if (key == "--courses") {
            args.coursesFile = value;
        } else if (key == "--periods") {
            args.periodsFile = value;
        } else if (key == "--selected-programs") {
            args.selectedProgramsFile = value;
        } else if (key == "--output") {
            args.outputFile = value;
        } else if (key == "--per-block-limit") {
            args.perBlockLimit = std::stoi(value);
        } else if (key == "--combined-limit") {
            args.combinedLimit = std::stoi(value);
        } else if (key == "--max-runtime-seconds") {
            args.maxRuntimeSeconds = std::stod(value);
        } else {
            throw std::runtime_error("Unknown argument: " + key);
        }
    }

    if (args.coursesFile.empty() || args.periodsFile.empty() ||
        args.selectedProgramsFile.empty() || args.outputFile.empty()) {
        throw std::runtime_error("Missing required arguments.");
    }

    return args;
}

int main(int argc, char* argv[]) {
    try {
        Arguments args = parseArguments(argc, argv);

        CourseFileParser courseParser;
        ExamPeriodFileParser periodParser;
        SelectedProgramsFileParser selectedProgramsParser;

        std::vector<Course> courses = courseParser.parse(args.coursesFile);
        std::vector<ExamPeriod> periods = periodParser.parse(args.periodsFile);
        std::vector<std::string> selectedPrograms = selectedProgramsParser.parse(args.selectedProgramsFile);

        SchedulingPreprocessor preprocessor(courses, periods, selectedPrograms);
        std::vector<SchedulingBlock> blocks = preprocessor.buildBlocks();

        if (blocks.empty()) {
            std::ofstream output(args.outputFile);
            output << "No exam blocks were found for the selected programs.\n";
            return 0;
        }

        std::vector<std::vector<std::vector<int>>> blockSolutions;
        for (const SchedulingBlock& block : blocks) {
            ExamScheduler scheduler(block, args.maxRuntimeSeconds);
            std::vector<std::vector<int>> solutions = scheduler.generateAll(args.perBlockLimit);

            if (solutions.empty()) {
                std::ofstream output(args.outputFile);
                output << "No valid schedules for block "
                       << block.semester << " / " << block.moed << ".\n";
                return 0;
            }

            blockSolutions.push_back(std::move(solutions));
        }

        OutputWriter writer;
        writer.writeCombinedSchedules(
            args.outputFile,
            blocks,
            blockSolutions,
            args.combinedLimit
        );

        std::cout << "Output written to " << args.outputFile << "\n";
        return 0;
    } catch (const ParseException& ex) {
        std::cerr << "Parse error: " << ex.what() << "\n";
        return 1;
    } catch (const SolverTimeoutException& ex) {
        std::cerr << "Timeout: " << ex.what() << "\n";
        return 1;
    } catch (const std::exception& ex) {
        printUsage();
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
