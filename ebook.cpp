#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <iomanip>
#include <numeric>


using namespace std;

struct RequestData {
    // Определяет, задан ли запрос (поле request непустое)
    explicit operator bool() const {
        return !request.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    string request;
	int user_id;
	uint16_t read_till_page_number;
};



class InputReader {
public:
	InputReader ()
		: page_read_{0}
		, users_amount_on_page_{0}
        , commands_{}
	{}

	void Parsing(string_view line) {
        auto command_description = ParseCommandDescription(line);
        if (command_description) {
            commands_.push_back(std::move(command_description));
        }
	}

	void ApplyCommands() {

        const RequestData& command_item = commands_.back();

        if (!command_item) {
            return;
        }

        if (command_item.request == "READ") {

            if (page_read_.size() <= command_item.user_id) {
                page_read_.resize(command_item.user_id + 1);
            }

            uint16_t page_number = command_item.read_till_page_number;

            if (users_amount_on_page_.size() <= page_number) {
                users_amount_on_page_.resize(page_number + 1);
            }
            ++users_amount_on_page_[page_number];

            if (!page_read_.empty() && page_read_.size() > command_item.user_id && page_read_[command_item.user_id] != 0) {
                --users_amount_on_page_[page_read_[command_item.user_id]];
            }
            page_read_[command_item.user_id] = page_number;
        }

        if (command_item.request == "CHEER") {
            double success_level = 0;

            if (!page_read_.empty() && page_read_.size() > command_item.user_id && page_read_[command_item.user_id] != 0) {
                int total_read_people = accumulate(users_amount_on_page_.begin(), users_amount_on_page_.end(), 0);
                if (total_read_people == 1) {
                    success_level = 1;
                } else {
                    int read_less_people = accumulate(users_amount_on_page_.begin(), next(users_amount_on_page_.begin(), page_read_[command_item.user_id]), 0);
                    success_level = read_less_people * 1.0 / (total_read_people - 1);
                }
            }
            cout << success_level << setprecision(6) << endl;
        }
            
        
    }
	


private:
	vector<int> page_read_;
	vector<int> users_amount_on_page_;

    vector<RequestData> commands_;

    const RequestData ParseCommandDescription(std::string_view line) const {

       
        vector<string_view> result;
            
            line.remove_prefix(min(line.find_first_not_of(" "), line.size()));

            while (!line.empty()) {
                // тут выполнен инвариант: line не начинается с пробела
                int64_t space = line.find(' ');

                // здесь можно избавиться от проверки на равенство npos
                // если space == npos, метод substr ограничит возвращаемый string_view концом строки
                result.push_back(line.substr(0, space));
                line.remove_prefix(min(line.find_first_not_of(" ", space), line.size()));
            }

            if (result.size() == 2) {
                return{ string(result[0]), stoi(string(result[1])), 0 };
        }


        return { string(result[0]),
            stoi(string(result[1])),
            static_cast<uint16_t>(stoi(string(result[2]))) };
    }
};

int main() {
	int request_count;
	cin >> request_count >> ws;

	InputReader reader;

	for (int i = 0; i < request_count; ++i) {
		string line;
		std::getline(cin, line);
       reader.Parsing(line);
       reader.ApplyCommands();
	}
	
}