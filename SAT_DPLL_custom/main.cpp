#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stack>
#include <algorithm>
#include <chrono>
#include <memory>
#include "NodeBoolTree.h"
#include "boolinterval.h"
#include "boolequation.h"
#include "BBV.h"
#include "./lib/Allocator/Allocator.h"

Allocator g_equation_allocator_(sizeof(BoolEquation), 1000, nullptr, "EquationAllocator");
Allocator g_node_allocator_(sizeof(NodeBoolTree), 2000, nullptr, "NodeAllocator");
Allocator g_interval_allocator_(sizeof(BoolInterval), 500, nullptr, "IntervalAllocator");
Allocator g_bbv_allocator_(sizeof(BBV), 1000, nullptr, "BBVAllocator");

/* Удаляет символы переноса строки из строки */
std::string TrimNewlines(const std::string& input_str_) {
    std::string result_ = input_str_;
    result_.erase(std::remove(result_.begin(), result_.end(), '\r'), result_.end());
    result_.erase(std::remove(result_.begin(), result_.end(), '\n'), result_.end());
    return result_;
}

/* Удаляет ведущие и завершающие пробелы */
std::string TrimWhitespace(const std::string& input_str_) {
    auto start_ = input_str_.begin();
    while (start_ != input_str_.end() && std::isspace(*start_)) ++start_;

    auto end_ = input_str_.end();
    do { --end_; } while (std::distance(start_, end_) > 0 && std::isspace(*end_));

    return std::string(start_, end_ + 1);
}

#ifdef USE_CUSTOM_ALLOCATOR
BoolEquation* AllocateEquation(BoolInterval** cnf_ptr_, BoolInterval* root_ptr_, int cnf_size_, int count_, BBV mask_, 
                              std::shared_ptr<BranchingStrategy> strategy_ptr_) {
    void* memory_ = g_equation_allocator_.Allocate(sizeof(BoolEquation));
    return new(memory_) BoolEquation(cnf_ptr_, root_ptr_, cnf_size_, count_, mask_, strategy_ptr_);
}

BoolEquation* AllocateEquationCopy(BoolEquation& equation_ref_) {
    void* memory_ = g_equation_allocator_.Allocate(sizeof(BoolEquation));
    return new(memory_) BoolEquation(equation_ref_);
}

NodeBoolTree* AllocateNode(BoolEquation* equation_ptr_) {
    void* memory_ = g_node_allocator_.Allocate(sizeof(NodeBoolTree));
    return new(memory_) NodeBoolTree(equation_ptr_);
}

BoolInterval* AllocateInterval(BBV& vec_ref_, BBV& dnc_ref_) {
    void* memory_ = g_interval_allocator_.Allocate(sizeof(BoolInterval));
    return new(memory_) BoolInterval(vec_ref_, dnc_ref_);
}

BoolInterval* AllocateInterval(const char* str_ptr_) {
    void* memory_ = g_interval_allocator_.Allocate(sizeof(BoolInterval));
    return new(memory_) BoolInterval(str_ptr_);
}

BBV* AllocateBBV(const char* str_ptr_) {
    void* memory_ = g_bbv_allocator_.Allocate(sizeof(BBV));
    return new(memory_) BBV(str_ptr_);
}

void DeallocateNode(NodeBoolTree* node_ptr_) {
    if (node_ptr_) {
        node_ptr_->~NodeBoolTree();
        g_node_allocator_.Deallocate(node_ptr_);
    }
}

void DeallocateEquation(BoolEquation* equation_ptr_) {
    if (equation_ptr_) {
        equation_ptr_->~BoolEquation();
        g_equation_allocator_.Deallocate(equation_ptr_);
    }
}

void DeallocateInterval(BoolInterval* interval_ptr_) {
    if (interval_ptr_) {
        interval_ptr_->~BoolInterval();
        g_interval_allocator_.Deallocate(interval_ptr_);
    }
}

void DeallocateBBV(BBV* bbv_ptr_) {
    if (bbv_ptr_) {
        bbv_ptr_->~BBV();
        g_bbv_allocator_.Deallocate(bbv_ptr_);
    }
}
#endif

int main(int argc, char* argv[]) {
    std::vector<std::string> file_lines_;
    std::string file_path_;
    
    std::shared_ptr<BranchingStrategy> strategy_ptr_ = std::make_shared<ColumnBranchingStrategy>();
    
    if (argc > 1) {
        file_path_ = argv[1];
        
        if (argc > 2) {
            std::string strategy_name_ = argv[2];
            if (strategy_name_ == "row") {
                strategy_ptr_ = std::make_shared<RowBranchingStrategy>();
                std::cout << "Using row branching strategy\n";
            }
        }
    } else {
        file_path_ = "./SatExamples/Sat_ex11_3.pla";
    }
    
    #ifdef USE_CUSTOM_ALLOCATOR
    std::cout << "Using custom allocators for class types\n";
    #else
    std::cout << "Using standard allocator\n";
    #endif
    
    std::ifstream input_file_(file_path_);
    auto time_start_ = std::chrono::high_resolution_clock::now();

    if (input_file_.is_open()) {
        std::string line_;
        while (std::getline(input_file_, line_)) {
            file_lines_.push_back(TrimNewlines(line_));
        }
        input_file_.close();

        int cnf_size_ = file_lines_.size();
        BoolInterval** cnf_ptr_ = new BoolInterval*[cnf_size_];
        int interval_rank_ = -1;

        if (!file_lines_.empty()) {
            interval_rank_ = TrimWhitespace(file_lines_[0]).length();
        }

        for (int i = 0; i < cnf_size_; ++i) {
            std::string line_content_ = TrimWhitespace(file_lines_[i]);
            #ifdef USE_CUSTOM_ALLOCATOR
            cnf_ptr_[i] = AllocateInterval(line_content_.c_str());
            #else
            cnf_ptr_[i] = new BoolInterval(line_content_.c_str());
            #endif
        }

        std::string root_vector_str_(interval_rank_, '0');
        std::string root_dnc_str_(interval_rank_, '1');

        #ifdef USE_CUSTOM_ALLOCATOR
        BBV* vec_ptr_ = AllocateBBV(root_vector_str_.c_str());
        BBV* dnc_ptr_ = AllocateBBV(root_dnc_str_.c_str());
        BoolInterval* root_ptr_ = AllocateInterval(*vec_ptr_, *dnc_ptr_);
        #else
        BBV vec(root_vector_str_.c_str());
        BBV dnc(root_dnc_str_.c_str());
        BoolInterval* root_ptr_ = new BoolInterval(vec, dnc);
        #endif

        BoolEquation* equation_ptr_;
        NodeBoolTree* root_node_ptr_;
        
        #ifdef USE_CUSTOM_ALLOCATOR
        equation_ptr_ = AllocateEquation(cnf_ptr_, root_ptr_, cnf_size_, cnf_size_, *vec_ptr_, strategy_ptr_);
        root_node_ptr_ = AllocateNode(equation_ptr_);
        #else
        equation_ptr_ = new BoolEquation(cnf_ptr_, root_ptr_, cnf_size_, cnf_size_, vec, strategy_ptr_);
        root_node_ptr_ = new NodeBoolTree(equation_ptr_);
        #endif

        bool solution_found_ = false;
        std::stack<NodeBoolTree*> node_stack_;
        node_stack_.push(root_node_ptr_);

        do {
            NodeBoolTree* current_node_ptr_ = node_stack_.top();

            if (current_node_ptr_->lt == nullptr && current_node_ptr_->rt == nullptr) { 
                BoolEquation* current_equation_ptr_ = current_node_ptr_->eq;
                bool processing_ = true;

                while (processing_) {
                    int rule_check_ = current_equation_ptr_->CheckRules();

                    switch (rule_check_) {
                        case 0: /* Нет решения */
                            node_stack_.pop();
                            processing_ = false;
                            break;

                        case 1: /* Найдено решение или упрощение */
                            if (current_equation_ptr_->count == 0 ||
                                current_equation_ptr_->mask.getWeight() == current_equation_ptr_->mask.getSize()) {
                                processing_ = false;
                                solution_found_ = true;

                                for (int i = 0; i < cnf_size_; ++i) {
                                    if (!cnf_ptr_[i]->isEqualComponent(*current_equation_ptr_->root)) {
                                        solution_found_ = false;
                                        node_stack_.pop();
                                        break;
                                    }
                                }
                            }
                            break;

                        case 2: /* Требуется ветвление */
                            int branch_index_ = current_equation_ptr_->ChooseBranchingIndex();

                            #ifdef USE_CUSTOM_ALLOCATOR
                            BoolEquation* eq0_ptr_ = AllocateEquationCopy(*current_equation_ptr_);
                            BoolEquation* eq1_ptr_ = AllocateEquationCopy(*current_equation_ptr_);
                            #else
                            BoolEquation* eq0_ptr_ = new BoolEquation(*current_equation_ptr_);
                            BoolEquation* eq1_ptr_ = new BoolEquation(*current_equation_ptr_);
                            #endif

                            eq0_ptr_->Simplify(branch_index_, '0');
                            eq1_ptr_->Simplify(branch_index_, '1');

                            #ifdef USE_CUSTOM_ALLOCATOR
                            NodeBoolTree* node0_ptr_ = AllocateNode(eq0_ptr_);
                            NodeBoolTree* node1_ptr_ = AllocateNode(eq1_ptr_);
                            #else
                            NodeBoolTree* node0_ptr_ = new NodeBoolTree(eq0_ptr_);
                            NodeBoolTree* node1_ptr_ = new NodeBoolTree(eq1_ptr_);
                            #endif

                            current_node_ptr_->lt = node0_ptr_;
                            current_node_ptr_->rt = node1_ptr_;

                            node_stack_.push(node1_ptr_);
                            node_stack_.push(node0_ptr_);
                            processing_ = false;
                            break;
                    }
                }
            } else {
                node_stack_.pop();
            }

        } while (node_stack_.size() > 1 && !solution_found_);

        auto time_end_ = std::chrono::high_resolution_clock::now();
        auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(time_end_ - time_start_).count();

        if (solution_found_) {
            std::cout << "Solution found:\n";
            std::cout << std::string(*node_stack_.top()->eq->root) << "\n";
        } else {
            std::cout << "No solution exists\n";
        }

        std::cout << "Execution time: " << duration_ << " μs\n";

        #ifdef USE_CUSTOM_ALLOCATOR
        /* Вывод статистики аллокаторов */
        #endif

        /* Освобождение ресурсов */
        #ifdef USE_CUSTOM_ALLOCATOR
        while (!node_stack_.empty()) {
            NodeBoolTree* node_ = node_stack_.top();
            node_stack_.pop();
            DeallocateEquation(node_->eq);
            DeallocateNode(node_);
        }

        for (int i = 0; i < cnf_size_; ++i) {
            DeallocateInterval(cnf_ptr_[i]);
        }
        DeallocateInterval(root_ptr_);
        DeallocateBBV(vec_ptr_);
        DeallocateBBV(dnc_ptr_);
        #else
        while (!node_stack_.empty()) {
            NodeBoolTree* node_ = node_stack_.top();
            node_stack_.pop();
            delete node_->eq;
            delete node_;
        }
        
        for (int i = 0; i < cnf_size_; ++i) {
            delete cnf_ptr_[i];
        }
        delete root_ptr_;
        #endif
        
        delete[] cnf_ptr_;

    } else {
        std::cout << "File not found\n";
    }

    return 0;
}