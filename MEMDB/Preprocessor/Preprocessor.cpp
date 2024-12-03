#include "Preprocessor.h"

Preprocessor::Preprocessor(std::string &query) {
    this->source = query;
    this->ok = true;
    q_subq = 0;
}

void Preprocessor::process_query(std::string &cur_command, std::vector<int> &flags, int flag) {
    int balance = 0;
    int i = 0;
    std::string subq_buf, curq_buf;
    std::vector<std::string> temp;
    std::vector<int> flags_temp;

    while (i < cur_command.size()) {
        //Subqueries will be marked with {}
        if (cur_command[i] == '{') {
            if(balance > 0) {
                subq_buf.push_back('{');
            }
            balance++;
        } else if (cur_command[i] == '}') {
            if(balance > 1) {
                subq_buf.push_back('}');
            }
            balance--;
            //Should be rewritten to work recursive
            if (balance == 0) {
                //this is a special sign for table which is a result of previous query
                curq_buf += (" _PREV" + std::to_string(this->q_subq + 1) + " ");
                this->q_subq++;
                temp.push_back(subq_buf);
                flags_temp.push_back(this->q_subq);
                subq_buf = "";
            }
        } else if (balance > 0) {
            subq_buf.push_back(cur_command[i]);
        } else if (balance == 0) {
            curq_buf.push_back(cur_command[i]);
        } else {
            this->ok = false;
            this->error = "Preprocessor: Syntax error. Wrong structure of subqueries.";
            break;
        }

        i++;
    }

//    reverse(temp.begin(), temp.end());
//    reverse(flags_temp.begin(), flags_temp.end());
    for (int i = 0; i < temp.size(); i++) {
        process_query(temp[i], flags, flags_temp[i]);
    }

    this->queries.push_back(curq_buf);
    flags.push_back(flag) ;
}

PreprocessorResult * Preprocessor::Parse() {
    std::string buf;
    std::vector<std::string> commands;
    int i = 0;

    //splitting string into queries with ";"
    while (i < this->source.size()) {
        if (this->source[i] != ';') {
            buf.push_back(this->source[i]);
        } else {
            commands.push_back(buf);
            buf = "";
        }
        i++;
    }

    //reverse(commands.begin(), commands.end());

    std::vector<int> flags;
    for (std::string &cur_command : commands) {
        process_query(cur_command, flags, 0);
    }

    if (this->ok) {
//        reverse(flags.begin(), flags.end());
//        reverse(this->queries.begin(), this->queries.end());
        //std::cout << flags.size() << std::endl;
        for (int j = 0; j < flags.size(); j++) {
            this->prom_res.push_back(std::make_pair(this->queries[j], flags[j]));
            //std::cout << this->queries[j] << " " << flags[j] << std::endl;
        }

    }

    PreprocessorResult *res = this->Preprocess();
    return res;
}

PreprocessorResult * Preprocessor::Preprocess() {
    std::vector<CommandsLine> res;

    //Preprocessing each query
    for (std::pair<std::string, int> cur_query: this->prom_res) {
        std::vector<Token *> cur_tokens;
        std::string buf;
        int i = 0;
        TokenFactory T = TokenFactory();

        //This long while loop splits string by " ", "\n" and creates tokens out of pieces
        while (i < cur_query.first.size()) {
            if (cur_query.first[i] == '\n' || cur_query.first[i] == ' ') {
                if (!buf.empty()) {
                    Token *r = T.make_token(buf);

                    if (r != nullptr) {
                        cur_tokens.push_back(r);
                    } else {
                        Token *rr = T.make_name(buf);

                        if (rr != nullptr) {
                            cur_tokens.push_back(rr);
                        } else {
                            this->ok = false;
                            this->error = "Preprocessor: uknown word " + buf;
                            break;
                        }
                    }

                    buf = "";
                }
            } else if (cur_query.first[i] == '(' || cur_query.first[i] == ')') {
                if (!buf.empty()) {
                    Token *r = T.make_token(buf);
                    //std::cout << "! " << buf << std::endl;
                    if (r != nullptr) {
                        cur_tokens.push_back(r);
                    } else {
                        Token *rr = T.make_name(buf);

                        if (rr != nullptr) {
                            cur_tokens.push_back(rr);
                        } else {
                            this->ok = false;
                            this->error = "Preprocessor: uknown word " + buf;
                            break;
                        }
                    }

                    buf = "";
                }

                buf.push_back(cur_query.first[i]);
                Token *rr = T.make_token(buf);
                cur_tokens.push_back(rr);
                buf = "";
            } else {
                buf.push_back(cur_query.first[i]);
            }

            i++;
        }

        if(!buf.empty()) {
            Token *r = T.make_token(buf);
            if (r != nullptr) {
                cur_tokens.push_back(r);
            } else {
                Token *rr = T.make_name(buf);

                if (rr != nullptr) {
                    cur_tokens.push_back(rr);
                } else {
                    this->ok = false;
                    this->error = "Preprocessor: uknown word " + buf;
                    break;
                }
            }

            buf = "";
        }

        //std::cout << "LOL " <<cur_query.second << std::endl;
        res.emplace_back(cur_tokens, cur_query.second);
    }

    auto prep_res = new PreprocessorResult();
    prep_res->ok = this->ok;
    prep_res->error = this->error;
    prep_res->data = res;
//    std::cout << "Prepr OK" << std::endl;
//    for(auto it : prep_res->data) {
//        std::cout << it.flag <<" ";
//    }
//    std::cout << std::endl;
//
//    for(auto it : this->prom_res) {
//        std::cout << it.second <<" ";
//    }
//    std::cout << std::endl;
    //std::cout << "Good by now" << std::endl;

    return prep_res;
}
