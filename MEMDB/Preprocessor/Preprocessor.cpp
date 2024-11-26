#include "Preprocessor.h"

Preprocessor::Preprocessor(std::string &query) {
    this->source = query;
    this->ok = true;
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

    std::vector<bool> flags;
    for (const std::string &cur_command : commands) {
        int q_subq = 0, balance = 0;
        i = 0;
        std::string subq_buf, curq_buf;
        std::vector<std::string> temp;

        while (i < cur_command.size()) {
            //Subqueries will be marked with {}
            if (cur_command[i] == '{') {
                balance++;
            } else if (cur_command[i] == '}') {
                balance--;
                //Should be rewritten to work recursive
                if (balance == 0) {
                    //this is a special sign for table which is a result of previous query
                    curq_buf += (" _PREV" + std::to_string(q_subq + 1) + " ");
                    q_subq++;
                    temp.push_back(subq_buf);
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

        reverse(temp.begin(), temp.end());
        for (auto &it : temp) {
            flags.push_back(false);
            this->queries.push_back(it);
        }

        this->queries.push_back(curq_buf);
        flags.push_back(true);
    }

    if (this->ok) {
        reverse(flags.begin(), flags.end());
        reverse(this->queries.begin(), this->queries.end());
        for (int j = 0; j < flags.size(); j++) {
            this->prom_res.push_back(std::make_pair(this->queries[j], flags[j]));
        }

    }

    PreprocessorResult *res = this->Preprocess();
    return res;
}

PreprocessorResult * Preprocessor::Preprocess() {
    std::vector<CommandsLine> res;

    //Preprocessing each query
    for (std::pair<std::string, bool> cur_query: this->prom_res) {
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

        res.emplace_back(cur_tokens, cur_query.second);
    }

    auto prep_res = new PreprocessorResult();
    prep_res->ok = this->ok;
    prep_res->error = this->error;
    prep_res->data = res;

    return prep_res;
}
