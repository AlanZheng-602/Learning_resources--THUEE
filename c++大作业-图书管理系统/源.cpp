#include<string>
#include<fstream>
#include<iostream>
#include<vector>
#include<ctime>
#include<cmath>
#include<algorithm>
#include<map>
#include<iomanip>
#include<sstream>
using namespace std;
int MaxUserId=1999999;
int MaxBookId=99998;
const string UserFile="users.dat";
const string BookFile="books.dat";
const string LogFile="logs.dat";
class Printable{
public:
    virtual void print()=0;
	virtual string ToFile() const=0;
};
class Time{
public:
    int year;
    int month;
    int date;
    int hr,mi;
	//方便文件读写的方法
	string ToFile()const{
		stringstream ss;
        ss << year << " " << month << " " << date << " " << hr << " " << mi;
        return ss.str();
	}
	static Time FromFile(const string& str){
		Time t;
		stringstream ss(str);
		ss >> t.year >> t.month >> t.date >> t.hr >> t.mi;
        return t;
	}
};
class BookCopy: public Printable{
private:
    int copyId;
    bool avail;
    int occu_by_userid;
public:
    BookCopy(int id) : copyId(id), avail(true), occu_by_userid(0) {}
	BookCopy(int id, bool a, int o):copyId(id),avail(a),occu_by_userid(o){}
    int GetCopyId() const { return copyId; }
    int GetOccuBy() const { return occu_by_userid; }
    bool IsAvailable() const { return avail; }
    void Borrow(int userId){
        if(avail){
            avail = false;
            occu_by_userid = userId;
        }
    }
    void Return(){
        avail = true;
        occu_by_userid = 0;
    }
    void print() override{
        cout << copyId <<"     "<< (avail ? "在架" : "出借")<<"       "<<(avail ? -1 : occu_by_userid)<<endl;
    }
	//文件读写
	string ToFile() const override{
		return to_string(copyId) + " " + to_string(avail) + " " + to_string(occu_by_userid);
	}
};
class Book: public Printable{
private:
    int id;
    string name;
    string author;
    string category;
    string key_word;
    string brief;
    vector<BookCopy> copies;
public:
	int GetId() const {return id;}
    vector<BookCopy>& GetCopies() { return copies; }
	const vector<BookCopy>& GetCopies() const{return copies;}
	string GetName(){return name;}
	string GetAuthor(){return author;}
	string GetCategory(){return category;}
	string GetKeyword(){return key_word;}
	string GetBrief(){return brief;}
	void SetId(int newId){id=newId;}
	void SetName(string newName) {name=newName;}
	void SetAuthor(string newAuthor) {author=newAuthor;}
	void SetCategory(string newCategory) {category=newCategory;}
	void SetKeyword(string newKeyword) {key_word=newKeyword;}
	void SetBrief(string newBrief) {brief=newBrief;}
    void AddCopy(int copyId) {
        copies.push_back(BookCopy(copyId));
    }
    void print() override {
		cout<<"书名ID:"<<id<<"     书名："<<name<<endl;
		cout<<"作者："<<author<<endl;
		cout<<"类别："<<category<<"   关键词："<<key_word<<endl;
		cout<<"简介："<<brief<<endl;
		cout << "---------------------" << endl;
		cout<<"书籍id"<<"     "<<"在架情况"<<"      "<<"借用人"<<endl;
		for(auto& copy : copies){
			copy.print();
		}
    }
	//文件格式输入输出，书籍基本信息读写不可控因素多（简介，作者长度等）故采用换行分割，可读性差
	string ToFile() const override {
        stringstream ss;
        ss << id << "\n" << name << "\n" << author << "\n" << category << "\n" << key_word << "\n" << brief << "\n";
        ss << copies.size() << "\n";
        for (const auto& copy : copies) {
            ss << copy.ToFile() << "\n";
        }
        return ss.str();
    }
    static Book FromFile(istream& in) {
        Book book;
        string line;
        getline(in, line);
        book.id = stoi(line);
        getline(in, book.name);
        getline(in, book.author);
        getline(in, book.category);
        getline(in, book.key_word);
        getline(in, book.brief);
        getline(in, line);
        int copyCount = stoi(line);
        for (int i = 0; i < copyCount; ++i) {
            getline(in, line);
            stringstream ss(line);
            int copyId, occu;
            bool avail;
            ss >> copyId >> avail >> occu;
            book.copies.emplace_back(copyId, avail, occu);
        }
        return book;
    }
};
class User: public Printable{
private:
    int id;
    string name;
    vector<int> BorrowedCopiesId;
public:
	User(){};
    User(int id, const string& name) : id(id), name(name) {}
    int GetId() const { return id; }
    string GetName() { return name; }
	void SetName(string newName) {name=newName;}
	const vector<int>& GetBorrowed() const { return BorrowedCopiesId; }
    void BorrowCopy(int copyId) { BorrowedCopiesId.push_back(copyId); }
    void ReturnCopy(int copyId) {
        auto it = find(BorrowedCopiesId.begin(), BorrowedCopiesId.end(), copyId);
        if(it != BorrowedCopiesId.end()) {
            BorrowedCopiesId.erase(it);
        }
    }
    void print() override {
        cout<<"用户名"<<name<<endl<<"用户id:"<<endl;
    }
	//文件格式输入输出
	string ToFile() const override {
        stringstream ss;
        ss << id << "\n" << name << "\n";
        ss << BorrowedCopiesId.size() << "\n";
        for (int id : BorrowedCopiesId) {
            ss << id << " ";
        }
        if (!BorrowedCopiesId.empty()) ss << "\n";
        return ss.str();
    }
    static User FromFile(istream& in) {
        User user;
        string line;
        getline(in, line);
        user.id = stoi(line);
        getline(in, user.name);
        getline(in, line);
        int borrowedCount = stoi(line);
        if (borrowedCount > 0) {
            getline(in, line);
            stringstream ss(line);
            for (int i = 0; i < borrowedCount; ++i) {
                int id;
                ss >> id;
                user.BorrowedCopiesId.push_back(id);
            }
        }
        return user;
    }
};
class Log : public Time, public Printable {
private:
    int userId;
    string action;
    int copyId;
    string bookTitle;
public:
	Log(){}
    Log(int userId, const string& action, int copyId, const string& bookTitle = "")
        : userId(userId), action(action), copyId(copyId), bookTitle(bookTitle) {
        // 获取当前系统时间
        time_t now = time(0);
        tm* ltm = localtime(&now);
        year = 1900 + ltm->tm_year;
        month = 1 + ltm->tm_mon;
        date = ltm->tm_mday;
        hr = ltm->tm_hour;
        mi = ltm->tm_min;
    }
    void print() override {// 格式化输出: YYYY-MM-DD HH:MM
		cout <<right;
        cout << setfill('0');
        cout << setw(4) << year << "-" 
             << setw(2) << month << "-" 
             << setw(2) << date << " "
             << setw(2) << hr << ":" 
             << setw(2) << mi << " ";
        cout << setfill(' '); // 恢复默认填充
        cout << "用户[" << userId << "] " << action << "了";
        if (!bookTitle.empty()) {
            cout << "《" << bookTitle << "》";
        }
        cout << " (副本ID: " << copyId << ")" << endl;
    }
    string GetTimeString() const {
        stringstream ss;
		ss <<right;
        ss << setfill('0');
        ss << setw(4) << year << "-" 
           << setw(2) << month << "-" 
           << setw(2) << date << " "
           << setw(2) << hr << ":" 
           << setw(2) << mi;
		ss<<setfill(' ');
        return ss.str();
    }
    string GetAction() const { return action; }
    int GetUserId() const { return userId; }
    int GetCopyId() const { return copyId; }
	//格式输入输出文件，用字符串处理的方法|使数据库可读性提升
	string ToFile() const override {
        stringstream ss;
        ss << Time::ToFile() << " | ";
        ss << userId << " | " << action << " | " << copyId << " | " << bookTitle;
        return ss.str();
    }
    static Log FromFileString(istream& in) {
        Log log;
        string line;
        getline(in, line);
		istringstream iss(line);
		vector<string> parts=split(iss.str()," | ");
        if(parts.size() != 5) {
			cout<<"数据库错误"<<endl;
		}
		Time t=Time::FromFile(parts[0]);
		static_cast<Time&>(log)=t;
		log.userId=stoi(parts[1]);
		log.action = parts[2];
		log.copyId = stoi(parts[3]);
		log.bookTitle = parts[4];
		return log;
    }
	static vector<string> split(const string& s, const string& div) {
		vector<string> buf;
		size_t pos = 0, next;
		while((next=s.find(div, pos)) != string::npos) {
			buf.push_back(s.substr(pos, next-pos));
			pos = next + div.length();
		}
		buf.push_back(s.substr(pos));
		return buf;
	}
};
class Library{
private:
    vector<Book> books;
    vector<User> users;
    vector<Log> logs;
   //仅在借还中需要使用到的书本id查找书本，多加一个if提高效率 ,放在private中防止外部调用
    BookCopy* LookupBookCopy(int copyId) {
        for(auto& book : books) {
			if(book.GetId()==int(copyId/100)){
				for(auto& copy : book.GetCopies()) {
					if(copy.GetCopyId() == copyId) {
						return &copy;
					}
				}
            }
		}
        return nullptr;
    }
	//仅在查询现在借阅时会用到的查询借阅时间
	string SearchBorrowTime(int uid, int cid){
		for(auto it=logs.rbegin();it!=logs.rend();it++){
			if(it->GetUserId()==uid && it->GetCopyId()==cid && it->GetAction()=="借出") return it->GetTimeString();
		}
		return "错误，无法在log中查询到日期";
	}
	//仅在需要“一段时间”的时间转长数字函数yyyymmddhhmm
	long long TimeToInt(const Time& t)const{
		return t.year * 100000000LL + t.month * 1000000LL +t.date * 10000LL +t.hr * 100LL +t.mi;
	}
public:
	//从数据库中将之前的数据加载
	void Load(){
		cout<<"加载数据中"<<endl;
		//加载用户数据
		ifstream uf(UserFile);
		if(uf.is_open()){
			while(uf.peek()!=EOF){
				try {
					User user = User::FromFile(uf);
					users.push_back(user);
					MaxUserId = max(MaxUserId, user.GetId());
                }catch (...) {
					cerr << "用户数据解析错误: " << endl;
					uf.clear();
					string jmp;
					getline(uf,jmp);
                }
			}
			MaxUserId++;
			uf.close();
			cout<<"用户信息加载完成，共"<<users.size()<<"条记录"<<endl;
		}else{
			cout << "用户数据文件不存在，请检查文件夹" << endl;
		}
		ifstream bf(BookFile);
		if (bf.is_open()) {
			while (bf.peek() != EOF) {
				try {
					Book book = Book::FromFile(bf);
					books.push_back(book);
					MaxBookId = max(MaxBookId, book.GetId());
				} catch (...) {
					cerr << "书籍数据解析错误" << endl;
					bf.clear();
					string jmp;
					getline(bf,jmp);
				}
			}
			MaxBookId++;
			bf.close();
			cout << "书籍数据加载完成，共" << books.size() << "条记录" << endl;
		} else {
			cout << "书籍数据文件不存在，请检查文件夹" << endl;
		}
		// 加载日志数据
		ifstream lf(LogFile);
		if (lf.is_open()) {
			string line;
			while (getline(lf, line)) {
				if (line.empty()) continue;
				try {
					istringstream iss(line);
					Log log = Log::FromFileString(iss);
					logs.push_back(log);
				} catch (...) {
					cerr << "日志数据解析错误: " << line << endl;
				}
			}
			lf.close();
			cout << "日志数据加载完成，共" << logs.size() << "条记录" << endl;
		} else {
			cout << "日志数据文件不存在，将创建新文件" << endl;
		}
	}
	//向数据库中存储
	void Save() {
		cout<<"正在保存本次操作数据，请不要退出"<<endl;
		cout<<"WARNING:现在强制退出程序将会导致不可控的数据丢失！"<<endl;
		// 保存用户数据
		ofstream uf(UserFile);
		if (uf.is_open()) {
			for (const User& user : users) {
				uf << user.ToFile();
				if (uf.fail()) {
					cerr << "写入用户数据失败: " << user.GetId() << endl;
				}
			}
			uf.close();
			cout << "用户数据保存完成，共" << users.size() << "条记录" << endl;
		} else {
			cerr << "无法打开用户数据文件: " << UserFile << endl;
		}
		// 保存书籍数据
		ofstream bf(BookFile);
		if (bf.is_open()) {
			for (const Book& book : books) {
				bf << book.ToFile();
				if (bf.fail()) {
					cerr << "写入书籍数据失败: " << book.GetId() << endl;
				}
			}
			bf.close();
			cout << "书籍数据保存完成，共" << books.size() << "条记录" << endl;
		} else {
			cerr << "无法打开书籍数据文件: " << BookFile << endl;
		}
		// 保存日志数据
		ofstream lf(LogFile);
		if (lf.is_open()) {
			for (const Log& log : logs) {
				lf << log.ToFile() << "\n";
				if (lf.fail()) {
					cerr << "写入日志数据失败" << endl;
				}
			}
			lf.close();
			cout << "日志数据保存完成，共" << logs.size() << "条记录" << endl;
		} else {
			cerr << "无法打开日志数据文件: " << LogFile << endl;
		}
		cout<<"数据存储完成";
	}
	//添加书种类和数量，编号方式为xxxxxxyy
	void AddBook(const Book& book, const int& num) {
		books.push_back(book);
		Book& addedBook=books.back();
		int bookId=addedBook.GetId();
		for(int i=0;i<num;i++){
			int copyId=bookId*100+i;
			addedBook.AddCopy(copyId);
		}
    }
	//删除某种书，参数为书id，copy数，主函数中或可以用书名查询id后再传参
	//这里需要注意，只有在库的书才可以安全删除，有多本书时，要优先删除在库的那些copies
	//如果查找了发现不够删除会弹出错误信息+确认强制删除信息，这样归还后也不会出现在库中
	void DeleteBook(int BookId, int copynum){
		Book* book=nullptr;
		for(auto& tmp:books){
			if(tmp.GetId()==BookId){
				book=&tmp;
				break;
			}
		}
		if(!book){cout<<"未找到此id的书籍，请检查输入";return;}
		vector<BookCopy>& copies=book->GetCopies();
		int validcc=0;
		int tot=0;
		for(auto& copy:copies){
			tot++;
			if(copy.IsAvailable()){validcc++;}
		}
		if(validcc<copynum){
			cout<<"警告：书籍ID "<< BookId <<"只有"<<validcc<< "本在库，但要求删除"<<copynum <<"本"<<endl;
			cout<<"是否强制删除这么多本书（y/n）"<<endl;
			char choice;
			cin>>choice;
			if(choice != 'y' && choice !='Y'){
				cout<<"操作取消"<<endl; return;
			}else{
				int delnum=min(copynum,tot);
				int deletedcc=0;
				auto it=copies.begin();
				while(it!=copies.end() && deletedcc<copynum){
					if(it->IsAvailable()){
						it=copies.erase(it);
						deletedcc++;
					}else{
						it++;
					}
				}
				it=copies.begin();
				for(int i=0;i<delnum-deletedcc;i++){
					ReturnBook(it->GetCopyId());
					it=copies.erase(it);
				}
				cout<<"已强制删除"<<endl;
			}	
		}else if(copynum>tot){
			cout<<"警告：书籍ID "<< BookId <<"共有"<<tot<< "本，但要求删除" << copynum <<"本"<<endl;
			cout<<"是否删除所有书（y/n）"<<endl;
			char choice;
			cin>>choice;
			if(choice=='y' || choice=='Y'){cout<<"已删除"<<endl; copies.clear();}
			else{cout<<"操作取消"<<endl;}
		}else{
			int deletedcc=0;
			auto it=copies.begin();
			while(it!=copies.end() && deletedcc<copynum){
				if(it->IsAvailable()){
					it=copies.erase(it);
					deletedcc++;
				}else{
					it++;
				}
			}
			cout<<"成功删除"<<deletedcc<<"个在库副本"<<endl;
		}
		if (copies.empty()){
            books.erase(remove_if(books.begin(),books.end(),[BookId](const Book& b) {return b.GetId() == BookId;}),books.end());
            cout <<"书籍ID "<<BookId <<"的所有副本已删除，书籍条目已自动移除"<<endl;
        }
	}
	//重写书籍信息
	void ReviseBook(int BookId,const string& newName = "",const string& newAuthor = "", const string& newCategory = "",const string& newKeyword = "",const string& newBrief = "") {
        Book* book = nullptr;
        for (auto& b : books) {
            if (b.GetId() == BookId) {
                book = &b;
                break;
            }
        }
        if (!book) {
            cout << "错误：未找到ID为 "<<BookId << " 的书籍" << endl;
            return;
        }
        string originalName = book->GetName();
        string originalAuthor = book->GetAuthor();
        if (!newName.empty()) book->SetName(newName);
        if (!newAuthor.empty()) book->SetAuthor(newAuthor);
        if (!newCategory.empty()) book->SetCategory(newCategory);
        if (!newKeyword.empty()) book->SetKeyword(newKeyword);
        if (!newBrief.empty()) book->SetBrief(newBrief);
        cout <<"BookId为"<<"BookId的书籍信息已更新" << endl;
        if (!newName.empty())cout << "书名: " << originalName << " -> " << book->GetName() << endl;
        if (!newAuthor.empty()) cout << "作者: " << originalAuthor << " -> " << book->GetAuthor() << endl;
        if (!newCategory.empty()) cout << "类别: " << book->GetCategory() << endl;
        if (!newKeyword.empty()) cout << "关键词: " << book->GetKeyword() << endl;
        if (!newBrief.empty()) cout << "简介: " << book->GetBrief() << endl;
    }
	//借书+自动生成日志
	bool BorrowBook(int userId, int copyId) {
		User* user = LookupUser(userId);
		if(!user) {cout<<"用户id错误"<<endl; return false;}
        BookCopy* copy = LookupBookCopy(copyId);
		if(!copy || !copy->IsAvailable()){cout<<"该书不可用"<<endl; return false;}
        string bookName="";
		if(Book* book=SearchById(int(copyId/100))){
			bookName=book->GetName();
		}
		copy->Borrow(userId);
        user->BorrowCopy(copyId);
        AddLog(Log(userId,"借出",copyId,bookName));
        return true;
    }
	//还书+自动生成日志
	bool ReturnBook(int copyId) {
        BookCopy* copy = LookupBookCopy(copyId);
		if(!copy || copy->IsAvailable()) {cout<<"该书已归还或已不存在"<<endl; return false;}
        int userId = copy->GetOccuBy();
		string bookName="";
		if(Book* book=SearchById(int(copyId/100))){
			bookName=book->GetName();
		}
        copy->Return();
        if(User* user = LookupUser(userId)) {
            user->ReturnCopy(copyId);
        }
        AddLog(Log(userId,"归还",copyId,bookName));
        return true;
    }
	//查书：书名查询（支持模糊查询，指有这堆字的都算）
	vector<Book*>SearchByName(const string& ipt){
		vector<Book*> results;
		for(auto& book:books){
			string title=book.GetName();
			transform(title.begin(),title.end(),title.begin(),::tolower);
			string key=ipt;
			transform(key.begin(),key.end(),key.begin(),::tolower);
			if(title.find(key)!=string::npos){
				results.push_back(&book);
			}
		}
		return results;
	}
	//查书：id查询（输入正确6位数图书id查询）
	Book* SearchById(int bookid){
		auto it=find_if(books.begin(),books.end(),[bookid](const Book& b){return b.GetId()==bookid;});
		return(it != books.end()) ? &(*it):nullptr;
	}
	//用名字添加用户，自动分配id
    void AddUser(const string& name){
        int id = ++MaxUserId;
        users.push_back(User(id, name));
		cout<<"user已成功添加，分配id为"<<id<<endl;
    }
	//删除用户并强制归还所有书:通过id查找删除
    void DeleteUser(int id){
        auto it = find_if(users.begin(), users.end(), [id](const User& u) { return u.GetId() == id; });
        if(it != users.end()) {//强制还书
            vector<int> borrowed = it->GetBorrowed(); 
            for(int copyId : borrowed) {
                ReturnBook(copyId);
            }
            users.erase(it);
			cout<<"用户 "<<id<<" 已成功删除，借阅书籍已自动归还";
        }else{
			cout<<"没有该id对应的用户！"<<endl;
			return;
		}
    }
	//通过id查找用户
    User* LookupUser(int id){
        auto it = find_if(users.begin(), users.end(),[id](const User& u) { return u.GetId() == id; });
        return (it != users.end()) ? &(*it) : nullptr;
    }
	//添加日志记录
	void AddLog(const Log& log){
		logs.push_back(log);
	}
	//查看日志记录
	void PrintLogs(int num=10){
		cout << "======= 最近" << num << "条操作日志 =======" << endl;
		int beg=max(0,(int)logs.size()-num);
		for(int i=logs.size()-1;i>=beg;i--){
			logs[i].print();
		}
	}    
	//查看用户在借图书
	void ViewUserBorrowed(int uid){
		User* user=LookupUser(uid);
		if(!user){
			cout<<"未找到该用户，请检查输入的userid"<<endl;
			return;
		}
		const vector<int>& borrowedCopies=user->GetBorrowed();
		if(borrowedCopies.empty()){
			cout<<"用户"<<user->GetName()<<"当前没有借书";
			return;
		}
		cout << "用户 "<<user->GetName()<<" (ID: "<< uid<< ") 的在借记录：" << endl;
		cout << "┌──────────┬───────────────────────┬───────────────────────┐" << endl;
		cout << "│ 副本ID   │ 书名                  │ 借阅时间              │" << endl;
		cout << "├──────────┼───────────────────────┼───────────────────────┤" << endl;
		for(int copyId:borrowedCopies){
			Book* book=SearchById(int(copyId/100));
			if (!book) {
                cout << "│ " << setw(9) << copyId << " │ 书籍已不存在" << endl;
                continue;
            }
			string BorrowTime=SearchBorrowTime(uid,copyId);
			string name = book->GetName();
            if (name.length() > 16) name = name.substr(0, 13) + "...";
            cout << "│ " << setw(8) << copyId << " │ "<< setw(21) << left << name << " │ "<< setw(21) <<BorrowTime<< " │" << endl;
		}
	}
	//查看借阅历史，按用户（需要从log中获取记录，因为用户还书之后不在book和user类中留下任何信息）
	void ViewUserHistory(int uid){
		User* user=LookupUser(uid);
		if(!user){
			cout<<"未找到该用户，请检查输入的userid"<<endl;
			return;
		}
		cout << "用户 "<<user->GetName()<<" (ID: "<< uid<< ") 的借阅记录：" << endl;
		cout << "┌──────────┬───────────────────────┬──────────────┬───────────────────────┐" << endl;
		cout << "│ 副本ID   │ 书名                  │ 操作         │ 操作时间              │" << endl;
		cout << "├──────────┼───────────────────────┼──────────────┼───────────────────────┤" << endl;
		for(auto it=logs.rbegin();it!=logs.rend();it++){
			if(it->GetUserId()==uid){
				int cid=it->GetCopyId();
				Book* book=SearchById(int (cid/100));
				string name;
				if(book==nullptr){
					name="书籍已不存在";
				}else{
					name=book->GetName();
				}
                if (name.length() > 16) name = name.substr(0, 13) + "...";
				cout << "│ " << setw(8) <<cid<< " │ "<< setw(21) << left << name << " │ "
					<< setw(12) << it->GetAction() << " │ "<< setw(21) <<it->GetTimeString()<< " │" << endl;
			}
		}
	}
	//查看借阅历史，按书（从log中搜索）
	void ViewBookHistory(int bid){
		Book* book=SearchById(bid);
		if(!book){
			cout<<"未找到该图书，请检查bid"<<endl;
			return;
		}
		cout << "书籍"<<book->GetName()<<" (ID: "<<bid<< ") 的借阅记录：" << endl;
       	cout << "┌──────────┬───────────────────────┬──────────────┬───────────────────────┐" << endl;
		cout << "│ 副本ID   │ 书名                  │ 操作         │ 操作时间              │" << endl;
		cout << "├──────────┼───────────────────────┼──────────────┼───────────────────────┤" << endl;
		for(auto it=logs.rbegin();it!=logs.rend();it++){
			int cid=it->GetCopyId();
			if(int(cid/100)==bid){
				User* user=LookupUser(it->GetUserId());
				string name;
				if(user==nullptr){
					name="该用户已不存在";
				}else{
					name=LookupUser(it->GetUserId())->GetName();
				}
				if (name.length() > 16) name = name.substr(0, 13) + "...";
				cout << "│ " << setw(8) <<cid<< " │ "<< setw(21) << left << name << " │ "
					<< setw(12) << it->GetAction() << " │ "<< setw(21) <<it->GetTimeString()<< " │" << endl;
			}
		}
	}
	//统计分析功能，按一段时间内最受欢迎书籍前n名作图
	vector<pair<Book*,int>> TopBooks(const Time& s, const Time& e, int num=10){
		long long start=TimeToInt(s);
		long long end=TimeToInt(e);
		map<int,int>BorrowCc;
		for(const Log& log: logs){//注意到log是按时间单调连续的，如果查找一段时间的话不用从前向后全部遍历计算
			if(TimeToInt(log)<start) continue;
			if(log.GetAction()!="借出") continue;
			long long  logTime=TimeToInt(log);
			if(logTime<=end){
				int BookId=log.GetCopyId()/100;
				BorrowCc[BookId]++;
			}else{
				break;
			}
		}
		vector<pair<int,int>> BorrowVec(BorrowCc.begin(),BorrowCc.end());
		sort(BorrowVec.begin(),BorrowVec.end(),[](const pair<int,int>&a,const pair<int,int>&b){return a.second>b.second;});
		vector<pair<Book*,int>> res;
		int cc=0;
		for(auto& b:BorrowVec){
			if(cc>=num)break;
			Book* book=SearchById(b.first);
			res.push_back(make_pair(book,b.second));
			cc++;
		}
		return res;
	}
	//统计分析功能，按一段时间内借阅最多的人前n名作图
	vector<pair<User*,int>> TopUsers(const Time& s, const Time& e, int num=10){
		long long start=TimeToInt(s);
		long long end=TimeToInt(e);
		map<int,int>UserCc;
		for(const Log& log: logs){
			if(TimeToInt(log)<start) continue;
			if(log.GetAction()!="借出") continue;
			long long logTime=TimeToInt(log);
			if(logTime<=end){
				int UserId=log.GetUserId();
				UserCc[UserId]++;
			}else{
				break;
			}
		}
		vector<pair<int,int>> UserVec(UserCc.begin(),UserCc.end());
		sort(UserVec.begin(),UserVec.end(),[](const pair<int,int>&a,const pair<int,int>&b){return a.second>b.second;});
		vector<pair<User*,int>> res;
		int cc=0;
		for(auto& u:UserVec){
			if(cc>=num)break;
			User* user=LookupUser(u.first);
			res.push_back(make_pair(user,u.second));
			cc++;
		}
		return res;
	}
};
//主函数中返回菜单对应的函数
void BackMenu(){
	cout<<"\n按任意键返回菜单...";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.get();
	system("cls");
}
//方便输入时间的辅助函数
Time InputTime() {
    Time t;
    cout << "  年份: "; cin >> t.year;
    cout << "  月份: "; cin >> t.month;
    cout << "  日期: "; cin >> t.date;
    cout << "  小时: "; cin >> t.hr;
    cout << "  分钟: "; cin >> t.mi;
    return t;
}
int main() {
    Library lib;
	lib.Load();
    int choice;
    while (true) {
		cout << "===== 图书管理系统主菜单 =====" << endl;
		cout << "【书籍管理】" << endl;
		cout << "  1. 添加书籍" << endl;
		cout << "  2. 删除书籍" << endl;
		cout << "  3. 修改书籍信息" << endl;
		cout << "\n【用户管理】" << endl;
		cout << "  4. 添加用户" << endl;
		cout << "  5. 删除用户" << endl;
		cout << "  6. 用户更名" <<endl;
		cout << "\n【借阅管理】" << endl;
		cout << "  7. 借书" << endl;
		cout << "  8. 还书" << endl;
		cout << "\n【查询功能】" << endl;
		cout << "  9. 查询书籍" << endl;
		cout << "  10. 查看用户借阅情况" << endl;
		cout << "  11. 查看书籍借阅历史" << endl;
		cout << "  12. 查看操作日志" << endl;
		cout << "\n【统计功能】"<<endl;
		cout << "  13. 统计最受欢迎书目"<<endl;
		cout << "  14. 统计借阅最多用户"<<endl;
		cout << "\n【系统操作】" << endl;
		cout << "  15. 退出系统并保存数据（WARNING:不经过此入口退出会导致本次所有操作不被储存）" << endl;
		cout << "============================" << endl;
		cout << "请输入选项(1-15): ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: {//添加书籍
                Book newBook;
				bool validipt=false;
                int numCopies;
                string name, author, category, keyword, brief;
                int newId = ++MaxBookId;
                newBook.SetId(newId);
                cout << "输入书名: "; getline(cin, name);
                cout << "输入作者: "; getline(cin, author);
                cout << "输入类别: "; getline(cin, category);
                cout << "输入关键词: "; getline(cin, keyword);
                cout << "输入简介: "; getline(cin, brief);
                while (!validipt) {
					cout << "输入副本数量(1-99): ";
					if (cin >> numCopies) {
						if (numCopies >= 1 && numCopies <= 99) {
							validipt=true;
						} else {cout << "错误：副本数量必须在1-99之间！" << endl;}
					} else {
					cout << "错误：请输入有效的数字！" << endl;
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(),'\n');
					}
				}
                newBook.SetName(name);
                newBook.SetAuthor(author);
                newBook.SetCategory(category);
                newBook.SetKeyword(keyword);
                newBook.SetBrief(brief);
                lib.AddBook(newBook, numCopies);
                cout << "添加成功! 书籍ID: " << newId << endl;
				BackMenu();
                break;
            }
            case 2: {  // 删除书籍
                int bookId, numCopies;
                cout << "输入要删除的书籍ID: "; cin >> bookId;
                cout << "输入要删除的副本数量: "; cin >> numCopies;
                lib.DeleteBook(bookId, numCopies);
				BackMenu();
                break;
            }
            case 3: {  // 修改书籍信息
                int bookId;
                cout << "输入要修改的书籍ID: "; cin >> bookId;
                cin.ignore();
                string name, author, category, keyword, brief;
                cout << "输入新书名(留空保持不变): "; getline(cin, name);
                cout << "输入新作者(留空保持不变): "; getline(cin, author);
                cout << "输入新类别(留空保持不变): "; getline(cin, category);
                cout << "输入新关键词(留空保持不变): "; getline(cin, keyword);
                cout << "输入新简介(留空保持不变): "; getline(cin, brief);
                lib.ReviseBook(bookId, name, author, category, keyword, brief);
				BackMenu();
                break;
            }
            case 4: {  // 添加用户
                string name;
                cout << "输入用户名: "; getline(cin, name);
                lib.AddUser(name);
				BackMenu();
                break;
            }
            case 5: {  // 删除用户
                int userId;
                cout << "输入要删除的用户ID: "; cin >> userId;
                lib.DeleteUser(userId);
				BackMenu();
                break;
            }
			case 6:{ //用户更名
				int uid;
				User* u;
				cout<<"输入用户ID:"; cin>>uid;
				u=lib.LookupUser(uid);
				if(u==nullptr){
					cout << "错误：未找到ID为 " << uid << " 的用户！" << endl;
					cout << "请检查ID是否正确或使用【4.添加用户】功能创建新用户。" << endl;
				}else{
					string oldName = u->GetName();
					string newName;
					cout << "当前用户名为: " << oldName << endl;
					cout << "请输入新用户名: ";
					cin.ignore(); 
					getline(cin, newName);
					if (!newName.empty()) {
						u->SetName(newName);
						cout << "用户更名成功！" << endl;
						cout <<"ID: "<<uid<<"的用户名从 "<< oldName << " 更改为 "<<newName<< endl;
					} else {
						cout << "错误：用户名不能为空！" << endl;
					}
				}
				BackMenu();
				break;
			}
            case 7: {  // 借书,正常管理系统中借书了应该是拿到那一本了，具体流程有待商榷，或明后天现场和助教讨论
                int userId, copyId;
				string name;
				cout<<"输入想要借阅的书名查询在架情况，或按下1跳过（直接进入副本ID借阅）"<<endl;
				//cin.ignore(numeric_limits<streamsize>::max(), '\n');
				getline(cin,name);
				if(name!="1"){
				system("cls");
				vector<Book*> results=lib.SearchByName(name);
				if (results.empty()) {
                        cout << "未找到匹配书籍" << endl;
                    }else{
                        cout << "\n===== 查询结果 =====" << endl;
                        for (Book* book : results) {
                            book->print();
                            cout << "---------------------" << endl;
                        }
                    }
				}
                cout << "输入想要借阅的副本ID: "; cin >> copyId;
                cout << "输入用户ID: "; cin >> userId;
                if (lib.BorrowBook(userId, copyId)) {
                    cout << "借书成功!" << endl;
                } else {
                    cout << "借书失败!" << endl;
                }	
				BackMenu();
                break;
			
            }
            case 8: {  // 还书
                int copyId;
                cout << "输入要归还的副本ID: "; cin >> copyId;
                if (lib.ReturnBook(copyId)) {
                    cout << "还书成功!" << endl;
                } else {
                    cout << "还书失败!" << endl;
                }
				BackMenu();
                break;
            }
            case 9: {  // 查询书籍
                cout << "查询方式: 1.按书名 2.按ID: ";
                int option; cin >> option;
                cin.ignore();
                if (option == 1) {
                    string name;
                    cout << "输入书名(支持模糊查询): "; getline(cin, name);
					system("cls");
                    vector<Book*> results = lib.SearchByName(name);
                    if (results.empty()) {
                        cout << "未找到匹配书籍" << endl;
                    } else {
                        cout << "\n===== 查询结果 =====" << endl;
                        for (Book* book : results) {
                            book->print();
                            cout << "---------------------" << endl;
                        }
                    }
                } else if (option == 2) {
                    int bookId;
                    cout << "输入书籍ID: "; cin >> bookId;
                    Book* book = lib.SearchById(bookId);
                    system("cls");
                    if (book) {
                        book->print();
                    } else {
                        cout << "未找到该ID的书籍" << endl;
                    }
                }
				BackMenu();
                break;
            }
            case 10: {  // 查看用户借阅情况
                int userId;
                cout << "输入用户ID: "; cin >> userId;
				system("cls");
                cout << "\n===== 当前借阅 =====\n";
                lib.ViewUserBorrowed(userId);
                cout << "\n===== 历史记录 =====\n";
                lib.ViewUserHistory(userId);
				BackMenu();
                break;
            }
            case 11: {  // 查看书籍借阅历史
                int bookId;
                cout << "输入书籍ID: "; cin >> bookId;
				system("cls");
                lib.ViewBookHistory(bookId);
				BackMenu();
                break;
            }
            case 12: {  // 查看操作日志
                int num;
                cout << "输入要查看的日志数量: "; 
                if (cin >> num) {
					system("cls");
                    lib.PrintLogs(num);
                } else {
                    cin.clear();
                    cin.ignore();
                    lib.PrintLogs();
                }
				BackMenu();
                break;
            }
			case 13:{//统计一段时间内受top n 受欢迎的书
				cout<<"热门书籍统计界面" <<endl;
				cout<<"输入统计起始时间："<<endl;
				Time s=InputTime();
				cout<<"输入统计结束时间："<<endl;
				Time e=InputTime();
				int topN=0;
				cout<<"请输入统计的热门书籍数量(1-20)"<<endl;
				cin>>topN;
				topN=max(1,min(20,topN));
				vector<pair<Book*,int>> Tops=lib.TopBooks(s,e,topN);
				if(Tops.empty()){
					cout<<"指定时间段内没有借书记录"<<endl;
				}else{
					system("cls");
					cout << "\n===== " << topN << "本最热门书籍 =====" << endl;
					cout << "排名 | 借阅次数 | 书籍ID | 书名" << endl;
					cout << "--------------------------------" << endl;
					for (unsigned int i = 0; i < Tops.size(); i++) {
						Book* book = Tops[i].first;
						int count = Tops[i].second;
						if(book==nullptr){
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) << "该书籍已不存在" << endl;
						}else{
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) << book->GetId() << " | "<< book->GetName() << endl;
						}
					}
					cout << "\n最受欢迎图书分布:"<< endl;
					int most = Tops[0].second;
					for (unsigned int i= 0;i<Tops.size();i++) {
						int count = Tops[i].second;
						int Length =count*50/most;
						if (Length == 0 && count > 0) Length = 1;
						cout << setw(2) << i+1 << " | "<<string(Length, '#')<< " " << count << endl;
					}
				}
				BackMenu();
				break;
			}
			case 14:{//统计一段时间内最卷的人
				cout<<"借阅最多的人统计界面";
				cout<<"输入统计起始时间："<<endl;
				Time s=InputTime();
				cout<<"输入统计结束时间："<<endl;
				Time e=InputTime();
				int topN=0;
				cout<<"请输入统计的热门借阅人数量(1-20)"<<endl;
				cin>>topN;
				topN=max(1,min(20,topN));
				vector<pair<User*,int>> Tops=lib.TopUsers(s,e,topN);
				if(Tops.empty()){
					cout<<"指定时间段内没有借书记录"<<endl;
				}else{
					system("cls");
					cout << "\n===== Top" << topN << "借阅最多读者 =====" << endl;
					cout << "排名 | 借阅次数 | 用户ID | 用户名" << endl;
					cout << "--------------------------------" << endl;
					for (unsigned int i = 0; i < Tops.size(); i++) {
						User* user = Tops[i].first;
						int count = Tops[i].second;
						if(user==nullptr){
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) <<"用户已不存在" << endl;
						}else{
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) << user->GetId() << " | "<< user->GetName() << endl;
						}
					}
					cout << "\n最受欢迎读者分布:"<< endl;
					int most = Tops[0].second;
					for (unsigned int i= 0;i<Tops.size();i++) {
						User* user = Tops[i].first;
						int count = Tops[i].second;
						int Length =count*50/most;
						if (Length == 0 && count > 0) Length = 1;
						cout << setw(2) << i+1 << " | "<<string(Length, '#')<< " " << count << endl;
					}
				}
				BackMenu();
				break;
			}
            case 15:  // 退出系统
				lib.Save();
                cout << "\n感谢使用，再见!" << endl;
                return 0; 
            default:
                cout << "无效选项，请重新输入!" << endl;
        }
    }
}