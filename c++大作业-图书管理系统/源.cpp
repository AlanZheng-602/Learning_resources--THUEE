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
	//�����ļ���д�ķ���
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
        cout << copyId <<"     "<< (avail ? "�ڼ�" : "����")<<"       "<<(avail ? -1 : occu_by_userid)<<endl;
    }
	//�ļ���д
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
		cout<<"����ID:"<<id<<"     ������"<<name<<endl;
		cout<<"���ߣ�"<<author<<endl;
		cout<<"���"<<category<<"   �ؼ��ʣ�"<<key_word<<endl;
		cout<<"��飺"<<brief<<endl;
		cout << "---------------------" << endl;
		cout<<"�鼮id"<<"     "<<"�ڼ����"<<"      "<<"������"<<endl;
		for(auto& copy : copies){
			copy.print();
		}
    }
	//�ļ���ʽ����������鼮������Ϣ��д���ɿ����ضࣨ��飬���߳��ȵȣ��ʲ��û��зָ�ɶ��Բ�
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
        cout<<"�û���"<<name<<endl<<"�û�id:"<<endl;
    }
	//�ļ���ʽ�������
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
        // ��ȡ��ǰϵͳʱ��
        time_t now = time(0);
        tm* ltm = localtime(&now);
        year = 1900 + ltm->tm_year;
        month = 1 + ltm->tm_mon;
        date = ltm->tm_mday;
        hr = ltm->tm_hour;
        mi = ltm->tm_min;
    }
    void print() override {// ��ʽ�����: YYYY-MM-DD HH:MM
		cout <<right;
        cout << setfill('0');
        cout << setw(4) << year << "-" 
             << setw(2) << month << "-" 
             << setw(2) << date << " "
             << setw(2) << hr << ":" 
             << setw(2) << mi << " ";
        cout << setfill(' '); // �ָ�Ĭ�����
        cout << "�û�[" << userId << "] " << action << "��";
        if (!bookTitle.empty()) {
            cout << "��" << bookTitle << "��";
        }
        cout << " (����ID: " << copyId << ")" << endl;
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
	//��ʽ��������ļ������ַ�������ķ���|ʹ���ݿ�ɶ�������
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
			cout<<"���ݿ����"<<endl;
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
   //���ڽ軹����Ҫʹ�õ����鱾id�����鱾�����һ��if���Ч�� ,����private�з�ֹ�ⲿ����
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
	//���ڲ�ѯ���ڽ���ʱ���õ��Ĳ�ѯ����ʱ��
	string SearchBorrowTime(int uid, int cid){
		for(auto it=logs.rbegin();it!=logs.rend();it++){
			if(it->GetUserId()==uid && it->GetCopyId()==cid && it->GetAction()=="���") return it->GetTimeString();
		}
		return "�����޷���log�в�ѯ������";
	}
	//������Ҫ��һ��ʱ�䡱��ʱ��ת�����ֺ���yyyymmddhhmm
	long long TimeToInt(const Time& t)const{
		return t.year * 100000000LL + t.month * 1000000LL +t.date * 10000LL +t.hr * 100LL +t.mi;
	}
public:
	//�����ݿ��н�֮ǰ�����ݼ���
	void Load(){
		cout<<"����������"<<endl;
		//�����û�����
		ifstream uf(UserFile);
		if(uf.is_open()){
			while(uf.peek()!=EOF){
				try {
					User user = User::FromFile(uf);
					users.push_back(user);
					MaxUserId = max(MaxUserId, user.GetId());
                }catch (...) {
					cerr << "�û����ݽ�������: " << endl;
					uf.clear();
					string jmp;
					getline(uf,jmp);
                }
			}
			MaxUserId++;
			uf.close();
			cout<<"�û���Ϣ������ɣ���"<<users.size()<<"����¼"<<endl;
		}else{
			cout << "�û������ļ������ڣ������ļ���" << endl;
		}
		ifstream bf(BookFile);
		if (bf.is_open()) {
			while (bf.peek() != EOF) {
				try {
					Book book = Book::FromFile(bf);
					books.push_back(book);
					MaxBookId = max(MaxBookId, book.GetId());
				} catch (...) {
					cerr << "�鼮���ݽ�������" << endl;
					bf.clear();
					string jmp;
					getline(bf,jmp);
				}
			}
			MaxBookId++;
			bf.close();
			cout << "�鼮���ݼ�����ɣ���" << books.size() << "����¼" << endl;
		} else {
			cout << "�鼮�����ļ������ڣ������ļ���" << endl;
		}
		// ������־����
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
					cerr << "��־���ݽ�������: " << line << endl;
				}
			}
			lf.close();
			cout << "��־���ݼ�����ɣ���" << logs.size() << "����¼" << endl;
		} else {
			cout << "��־�����ļ������ڣ����������ļ�" << endl;
		}
	}
	//�����ݿ��д洢
	void Save() {
		cout<<"���ڱ��汾�β������ݣ��벻Ҫ�˳�"<<endl;
		cout<<"WARNING:����ǿ���˳����򽫻ᵼ�²��ɿص����ݶ�ʧ��"<<endl;
		// �����û�����
		ofstream uf(UserFile);
		if (uf.is_open()) {
			for (const User& user : users) {
				uf << user.ToFile();
				if (uf.fail()) {
					cerr << "д���û�����ʧ��: " << user.GetId() << endl;
				}
			}
			uf.close();
			cout << "�û����ݱ�����ɣ���" << users.size() << "����¼" << endl;
		} else {
			cerr << "�޷����û������ļ�: " << UserFile << endl;
		}
		// �����鼮����
		ofstream bf(BookFile);
		if (bf.is_open()) {
			for (const Book& book : books) {
				bf << book.ToFile();
				if (bf.fail()) {
					cerr << "д���鼮����ʧ��: " << book.GetId() << endl;
				}
			}
			bf.close();
			cout << "�鼮���ݱ�����ɣ���" << books.size() << "����¼" << endl;
		} else {
			cerr << "�޷����鼮�����ļ�: " << BookFile << endl;
		}
		// ������־����
		ofstream lf(LogFile);
		if (lf.is_open()) {
			for (const Log& log : logs) {
				lf << log.ToFile() << "\n";
				if (lf.fail()) {
					cerr << "д����־����ʧ��" << endl;
				}
			}
			lf.close();
			cout << "��־���ݱ�����ɣ���" << logs.size() << "����¼" << endl;
		} else {
			cerr << "�޷�����־�����ļ�: " << LogFile << endl;
		}
		cout<<"���ݴ洢���";
	}
	//������������������ŷ�ʽΪxxxxxxyy
	void AddBook(const Book& book, const int& num) {
		books.push_back(book);
		Book& addedBook=books.back();
		int bookId=addedBook.GetId();
		for(int i=0;i<num;i++){
			int copyId=bookId*100+i;
			addedBook.AddCopy(copyId);
		}
    }
	//ɾ��ĳ���飬����Ϊ��id��copy�����������л������������ѯid���ٴ���
	//������Ҫע�⣬ֻ���ڿ����ſ��԰�ȫɾ�����ж౾��ʱ��Ҫ����ɾ���ڿ����Щcopies
	//��������˷��ֲ���ɾ���ᵯ��������Ϣ+ȷ��ǿ��ɾ����Ϣ�������黹��Ҳ��������ڿ���
	void DeleteBook(int BookId, int copynum){
		Book* book=nullptr;
		for(auto& tmp:books){
			if(tmp.GetId()==BookId){
				book=&tmp;
				break;
			}
		}
		if(!book){cout<<"δ�ҵ���id���鼮����������";return;}
		vector<BookCopy>& copies=book->GetCopies();
		int validcc=0;
		int tot=0;
		for(auto& copy:copies){
			tot++;
			if(copy.IsAvailable()){validcc++;}
		}
		if(validcc<copynum){
			cout<<"���棺�鼮ID "<< BookId <<"ֻ��"<<validcc<< "���ڿ⣬��Ҫ��ɾ��"<<copynum <<"��"<<endl;
			cout<<"�Ƿ�ǿ��ɾ����ô�౾�飨y/n��"<<endl;
			char choice;
			cin>>choice;
			if(choice != 'y' && choice !='Y'){
				cout<<"����ȡ��"<<endl; return;
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
				cout<<"��ǿ��ɾ��"<<endl;
			}	
		}else if(copynum>tot){
			cout<<"���棺�鼮ID "<< BookId <<"����"<<tot<< "������Ҫ��ɾ��" << copynum <<"��"<<endl;
			cout<<"�Ƿ�ɾ�������飨y/n��"<<endl;
			char choice;
			cin>>choice;
			if(choice=='y' || choice=='Y'){cout<<"��ɾ��"<<endl; copies.clear();}
			else{cout<<"����ȡ��"<<endl;}
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
			cout<<"�ɹ�ɾ��"<<deletedcc<<"���ڿ⸱��"<<endl;
		}
		if (copies.empty()){
            books.erase(remove_if(books.begin(),books.end(),[BookId](const Book& b) {return b.GetId() == BookId;}),books.end());
            cout <<"�鼮ID "<<BookId <<"�����и�����ɾ�����鼮��Ŀ���Զ��Ƴ�"<<endl;
        }
	}
	//��д�鼮��Ϣ
	void ReviseBook(int BookId,const string& newName = "",const string& newAuthor = "", const string& newCategory = "",const string& newKeyword = "",const string& newBrief = "") {
        Book* book = nullptr;
        for (auto& b : books) {
            if (b.GetId() == BookId) {
                book = &b;
                break;
            }
        }
        if (!book) {
            cout << "����δ�ҵ�IDΪ "<<BookId << " ���鼮" << endl;
            return;
        }
        string originalName = book->GetName();
        string originalAuthor = book->GetAuthor();
        if (!newName.empty()) book->SetName(newName);
        if (!newAuthor.empty()) book->SetAuthor(newAuthor);
        if (!newCategory.empty()) book->SetCategory(newCategory);
        if (!newKeyword.empty()) book->SetKeyword(newKeyword);
        if (!newBrief.empty()) book->SetBrief(newBrief);
        cout <<"BookIdΪ"<<"BookId���鼮��Ϣ�Ѹ���" << endl;
        if (!newName.empty())cout << "����: " << originalName << " -> " << book->GetName() << endl;
        if (!newAuthor.empty()) cout << "����: " << originalAuthor << " -> " << book->GetAuthor() << endl;
        if (!newCategory.empty()) cout << "���: " << book->GetCategory() << endl;
        if (!newKeyword.empty()) cout << "�ؼ���: " << book->GetKeyword() << endl;
        if (!newBrief.empty()) cout << "���: " << book->GetBrief() << endl;
    }
	//����+�Զ�������־
	bool BorrowBook(int userId, int copyId) {
		User* user = LookupUser(userId);
		if(!user) {cout<<"�û�id����"<<endl; return false;}
        BookCopy* copy = LookupBookCopy(copyId);
		if(!copy || !copy->IsAvailable()){cout<<"���鲻����"<<endl; return false;}
        string bookName="";
		if(Book* book=SearchById(int(copyId/100))){
			bookName=book->GetName();
		}
		copy->Borrow(userId);
        user->BorrowCopy(copyId);
        AddLog(Log(userId,"���",copyId,bookName));
        return true;
    }
	//����+�Զ�������־
	bool ReturnBook(int copyId) {
        BookCopy* copy = LookupBookCopy(copyId);
		if(!copy || copy->IsAvailable()) {cout<<"�����ѹ黹���Ѳ�����"<<endl; return false;}
        int userId = copy->GetOccuBy();
		string bookName="";
		if(Book* book=SearchById(int(copyId/100))){
			bookName=book->GetName();
		}
        copy->Return();
        if(User* user = LookupUser(userId)) {
            user->ReturnCopy(copyId);
        }
        AddLog(Log(userId,"�黹",copyId,bookName));
        return true;
    }
	//���飺������ѯ��֧��ģ����ѯ��ָ������ֵĶ��㣩
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
	//���飺id��ѯ��������ȷ6λ��ͼ��id��ѯ��
	Book* SearchById(int bookid){
		auto it=find_if(books.begin(),books.end(),[bookid](const Book& b){return b.GetId()==bookid;});
		return(it != books.end()) ? &(*it):nullptr;
	}
	//����������û����Զ�����id
    void AddUser(const string& name){
        int id = ++MaxUserId;
        users.push_back(User(id, name));
		cout<<"user�ѳɹ���ӣ�����idΪ"<<id<<endl;
    }
	//ɾ���û���ǿ�ƹ黹������:ͨ��id����ɾ��
    void DeleteUser(int id){
        auto it = find_if(users.begin(), users.end(), [id](const User& u) { return u.GetId() == id; });
        if(it != users.end()) {//ǿ�ƻ���
            vector<int> borrowed = it->GetBorrowed(); 
            for(int copyId : borrowed) {
                ReturnBook(copyId);
            }
            users.erase(it);
			cout<<"�û� "<<id<<" �ѳɹ�ɾ���������鼮���Զ��黹";
        }else{
			cout<<"û�и�id��Ӧ���û���"<<endl;
			return;
		}
    }
	//ͨ��id�����û�
    User* LookupUser(int id){
        auto it = find_if(users.begin(), users.end(),[id](const User& u) { return u.GetId() == id; });
        return (it != users.end()) ? &(*it) : nullptr;
    }
	//�����־��¼
	void AddLog(const Log& log){
		logs.push_back(log);
	}
	//�鿴��־��¼
	void PrintLogs(int num=10){
		cout << "======= ���" << num << "��������־ =======" << endl;
		int beg=max(0,(int)logs.size()-num);
		for(int i=logs.size()-1;i>=beg;i--){
			logs[i].print();
		}
	}    
	//�鿴�û��ڽ�ͼ��
	void ViewUserBorrowed(int uid){
		User* user=LookupUser(uid);
		if(!user){
			cout<<"δ�ҵ����û������������userid"<<endl;
			return;
		}
		const vector<int>& borrowedCopies=user->GetBorrowed();
		if(borrowedCopies.empty()){
			cout<<"�û�"<<user->GetName()<<"��ǰû�н���";
			return;
		}
		cout << "�û� "<<user->GetName()<<" (ID: "<< uid<< ") ���ڽ��¼��" << endl;
		cout << "�����������������������Щ����������������������������������������������Щ�����������������������������������������������" << endl;
		cout << "�� ����ID   �� ����                  �� ����ʱ��              ��" << endl;
		cout << "�����������������������੤���������������������������������������������੤����������������������������������������������" << endl;
		for(int copyId:borrowedCopies){
			Book* book=SearchById(int(copyId/100));
			if (!book) {
                cout << "�� " << setw(9) << copyId << " �� �鼮�Ѳ�����" << endl;
                continue;
            }
			string BorrowTime=SearchBorrowTime(uid,copyId);
			string name = book->GetName();
            if (name.length() > 16) name = name.substr(0, 13) + "...";
            cout << "�� " << setw(8) << copyId << " �� "<< setw(21) << left << name << " �� "<< setw(21) <<BorrowTime<< " ��" << endl;
		}
	}
	//�鿴������ʷ�����û�����Ҫ��log�л�ȡ��¼����Ϊ�û�����֮����book��user���������κ���Ϣ��
	void ViewUserHistory(int uid){
		User* user=LookupUser(uid);
		if(!user){
			cout<<"δ�ҵ����û������������userid"<<endl;
			return;
		}
		cout << "�û� "<<user->GetName()<<" (ID: "<< uid<< ") �Ľ��ļ�¼��" << endl;
		cout << "�����������������������Щ����������������������������������������������Щ����������������������������Щ�����������������������������������������������" << endl;
		cout << "�� ����ID   �� ����                  �� ����         �� ����ʱ��              ��" << endl;
		cout << "�����������������������੤���������������������������������������������੤���������������������������੤����������������������������������������������" << endl;
		for(auto it=logs.rbegin();it!=logs.rend();it++){
			if(it->GetUserId()==uid){
				int cid=it->GetCopyId();
				Book* book=SearchById(int (cid/100));
				string name;
				if(book==nullptr){
					name="�鼮�Ѳ�����";
				}else{
					name=book->GetName();
				}
                if (name.length() > 16) name = name.substr(0, 13) + "...";
				cout << "�� " << setw(8) <<cid<< " �� "<< setw(21) << left << name << " �� "
					<< setw(12) << it->GetAction() << " �� "<< setw(21) <<it->GetTimeString()<< " ��" << endl;
			}
		}
	}
	//�鿴������ʷ�����飨��log��������
	void ViewBookHistory(int bid){
		Book* book=SearchById(bid);
		if(!book){
			cout<<"δ�ҵ���ͼ�飬����bid"<<endl;
			return;
		}
		cout << "�鼮"<<book->GetName()<<" (ID: "<<bid<< ") �Ľ��ļ�¼��" << endl;
       	cout << "�����������������������Щ����������������������������������������������Щ����������������������������Щ�����������������������������������������������" << endl;
		cout << "�� ����ID   �� ����                  �� ����         �� ����ʱ��              ��" << endl;
		cout << "�����������������������੤���������������������������������������������੤���������������������������੤����������������������������������������������" << endl;
		for(auto it=logs.rbegin();it!=logs.rend();it++){
			int cid=it->GetCopyId();
			if(int(cid/100)==bid){
				User* user=LookupUser(it->GetUserId());
				string name;
				if(user==nullptr){
					name="���û��Ѳ�����";
				}else{
					name=LookupUser(it->GetUserId())->GetName();
				}
				if (name.length() > 16) name = name.substr(0, 13) + "...";
				cout << "�� " << setw(8) <<cid<< " �� "<< setw(21) << left << name << " �� "
					<< setw(12) << it->GetAction() << " �� "<< setw(21) <<it->GetTimeString()<< " ��" << endl;
			}
		}
	}
	//ͳ�Ʒ������ܣ���һ��ʱ�������ܻ�ӭ�鼮ǰn����ͼ
	vector<pair<Book*,int>> TopBooks(const Time& s, const Time& e, int num=10){
		long long start=TimeToInt(s);
		long long end=TimeToInt(e);
		map<int,int>BorrowCc;
		for(const Log& log: logs){//ע�⵽log�ǰ�ʱ�䵥�������ģ��������һ��ʱ��Ļ����ô�ǰ���ȫ����������
			if(TimeToInt(log)<start) continue;
			if(log.GetAction()!="���") continue;
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
	//ͳ�Ʒ������ܣ���һ��ʱ���ڽ���������ǰn����ͼ
	vector<pair<User*,int>> TopUsers(const Time& s, const Time& e, int num=10){
		long long start=TimeToInt(s);
		long long end=TimeToInt(e);
		map<int,int>UserCc;
		for(const Log& log: logs){
			if(TimeToInt(log)<start) continue;
			if(log.GetAction()!="���") continue;
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
//�������з��ز˵���Ӧ�ĺ���
void BackMenu(){
	cout<<"\n����������ز˵�...";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.get();
	system("cls");
}
//��������ʱ��ĸ�������
Time InputTime() {
    Time t;
    cout << "  ���: "; cin >> t.year;
    cout << "  �·�: "; cin >> t.month;
    cout << "  ����: "; cin >> t.date;
    cout << "  Сʱ: "; cin >> t.hr;
    cout << "  ����: "; cin >> t.mi;
    return t;
}
int main() {
    Library lib;
	lib.Load();
    int choice;
    while (true) {
		cout << "===== ͼ�����ϵͳ���˵� =====" << endl;
		cout << "���鼮����" << endl;
		cout << "  1. ����鼮" << endl;
		cout << "  2. ɾ���鼮" << endl;
		cout << "  3. �޸��鼮��Ϣ" << endl;
		cout << "\n���û�����" << endl;
		cout << "  4. ����û�" << endl;
		cout << "  5. ɾ���û�" << endl;
		cout << "  6. �û�����" <<endl;
		cout << "\n�����Ĺ���" << endl;
		cout << "  7. ����" << endl;
		cout << "  8. ����" << endl;
		cout << "\n����ѯ���ܡ�" << endl;
		cout << "  9. ��ѯ�鼮" << endl;
		cout << "  10. �鿴�û��������" << endl;
		cout << "  11. �鿴�鼮������ʷ" << endl;
		cout << "  12. �鿴������־" << endl;
		cout << "\n��ͳ�ƹ��ܡ�"<<endl;
		cout << "  13. ͳ�����ܻ�ӭ��Ŀ"<<endl;
		cout << "  14. ͳ�ƽ�������û�"<<endl;
		cout << "\n��ϵͳ������" << endl;
		cout << "  15. �˳�ϵͳ���������ݣ�WARNING:������������˳��ᵼ�±������в����������棩" << endl;
		cout << "============================" << endl;
		cout << "������ѡ��(1-15): ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: {//����鼮
                Book newBook;
				bool validipt=false;
                int numCopies;
                string name, author, category, keyword, brief;
                int newId = ++MaxBookId;
                newBook.SetId(newId);
                cout << "��������: "; getline(cin, name);
                cout << "��������: "; getline(cin, author);
                cout << "�������: "; getline(cin, category);
                cout << "����ؼ���: "; getline(cin, keyword);
                cout << "������: "; getline(cin, brief);
                while (!validipt) {
					cout << "���븱������(1-99): ";
					if (cin >> numCopies) {
						if (numCopies >= 1 && numCopies <= 99) {
							validipt=true;
						} else {cout << "���󣺸�������������1-99֮�䣡" << endl;}
					} else {
					cout << "������������Ч�����֣�" << endl;
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
                cout << "��ӳɹ�! �鼮ID: " << newId << endl;
				BackMenu();
                break;
            }
            case 2: {  // ɾ���鼮
                int bookId, numCopies;
                cout << "����Ҫɾ�����鼮ID: "; cin >> bookId;
                cout << "����Ҫɾ���ĸ�������: "; cin >> numCopies;
                lib.DeleteBook(bookId, numCopies);
				BackMenu();
                break;
            }
            case 3: {  // �޸��鼮��Ϣ
                int bookId;
                cout << "����Ҫ�޸ĵ��鼮ID: "; cin >> bookId;
                cin.ignore();
                string name, author, category, keyword, brief;
                cout << "����������(���ձ��ֲ���): "; getline(cin, name);
                cout << "����������(���ձ��ֲ���): "; getline(cin, author);
                cout << "���������(���ձ��ֲ���): "; getline(cin, category);
                cout << "�����¹ؼ���(���ձ��ֲ���): "; getline(cin, keyword);
                cout << "�����¼��(���ձ��ֲ���): "; getline(cin, brief);
                lib.ReviseBook(bookId, name, author, category, keyword, brief);
				BackMenu();
                break;
            }
            case 4: {  // ����û�
                string name;
                cout << "�����û���: "; getline(cin, name);
                lib.AddUser(name);
				BackMenu();
                break;
            }
            case 5: {  // ɾ���û�
                int userId;
                cout << "����Ҫɾ�����û�ID: "; cin >> userId;
                lib.DeleteUser(userId);
				BackMenu();
                break;
            }
			case 6:{ //�û�����
				int uid;
				User* u;
				cout<<"�����û�ID:"; cin>>uid;
				u=lib.LookupUser(uid);
				if(u==nullptr){
					cout << "����δ�ҵ�IDΪ " << uid << " ���û���" << endl;
					cout << "����ID�Ƿ���ȷ��ʹ�á�4.����û������ܴ������û���" << endl;
				}else{
					string oldName = u->GetName();
					string newName;
					cout << "��ǰ�û���Ϊ: " << oldName << endl;
					cout << "���������û���: ";
					cin.ignore(); 
					getline(cin, newName);
					if (!newName.empty()) {
						u->SetName(newName);
						cout << "�û������ɹ���" << endl;
						cout <<"ID: "<<uid<<"���û����� "<< oldName << " ����Ϊ "<<newName<< endl;
					} else {
						cout << "�����û�������Ϊ�գ�" << endl;
					}
				}
				BackMenu();
				break;
			}
            case 7: {  // ����,��������ϵͳ�н�����Ӧ�����õ���һ���ˣ����������д���ȶ�����������ֳ�����������
                int userId, copyId;
				string name;
				cout<<"������Ҫ���ĵ�������ѯ�ڼ����������1������ֱ�ӽ��븱��ID���ģ�"<<endl;
				//cin.ignore(numeric_limits<streamsize>::max(), '\n');
				getline(cin,name);
				if(name!="1"){
				system("cls");
				vector<Book*> results=lib.SearchByName(name);
				if (results.empty()) {
                        cout << "δ�ҵ�ƥ���鼮" << endl;
                    }else{
                        cout << "\n===== ��ѯ��� =====" << endl;
                        for (Book* book : results) {
                            book->print();
                            cout << "---------------------" << endl;
                        }
                    }
				}
                cout << "������Ҫ���ĵĸ���ID: "; cin >> copyId;
                cout << "�����û�ID: "; cin >> userId;
                if (lib.BorrowBook(userId, copyId)) {
                    cout << "����ɹ�!" << endl;
                } else {
                    cout << "����ʧ��!" << endl;
                }	
				BackMenu();
                break;
			
            }
            case 8: {  // ����
                int copyId;
                cout << "����Ҫ�黹�ĸ���ID: "; cin >> copyId;
                if (lib.ReturnBook(copyId)) {
                    cout << "����ɹ�!" << endl;
                } else {
                    cout << "����ʧ��!" << endl;
                }
				BackMenu();
                break;
            }
            case 9: {  // ��ѯ�鼮
                cout << "��ѯ��ʽ: 1.������ 2.��ID: ";
                int option; cin >> option;
                cin.ignore();
                if (option == 1) {
                    string name;
                    cout << "��������(֧��ģ����ѯ): "; getline(cin, name);
					system("cls");
                    vector<Book*> results = lib.SearchByName(name);
                    if (results.empty()) {
                        cout << "δ�ҵ�ƥ���鼮" << endl;
                    } else {
                        cout << "\n===== ��ѯ��� =====" << endl;
                        for (Book* book : results) {
                            book->print();
                            cout << "---------------------" << endl;
                        }
                    }
                } else if (option == 2) {
                    int bookId;
                    cout << "�����鼮ID: "; cin >> bookId;
                    Book* book = lib.SearchById(bookId);
                    system("cls");
                    if (book) {
                        book->print();
                    } else {
                        cout << "δ�ҵ���ID���鼮" << endl;
                    }
                }
				BackMenu();
                break;
            }
            case 10: {  // �鿴�û��������
                int userId;
                cout << "�����û�ID: "; cin >> userId;
				system("cls");
                cout << "\n===== ��ǰ���� =====\n";
                lib.ViewUserBorrowed(userId);
                cout << "\n===== ��ʷ��¼ =====\n";
                lib.ViewUserHistory(userId);
				BackMenu();
                break;
            }
            case 11: {  // �鿴�鼮������ʷ
                int bookId;
                cout << "�����鼮ID: "; cin >> bookId;
				system("cls");
                lib.ViewBookHistory(bookId);
				BackMenu();
                break;
            }
            case 12: {  // �鿴������־
                int num;
                cout << "����Ҫ�鿴����־����: "; 
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
			case 13:{//ͳ��һ��ʱ������top n �ܻ�ӭ����
				cout<<"�����鼮ͳ�ƽ���" <<endl;
				cout<<"����ͳ����ʼʱ�䣺"<<endl;
				Time s=InputTime();
				cout<<"����ͳ�ƽ���ʱ�䣺"<<endl;
				Time e=InputTime();
				int topN=0;
				cout<<"������ͳ�Ƶ������鼮����(1-20)"<<endl;
				cin>>topN;
				topN=max(1,min(20,topN));
				vector<pair<Book*,int>> Tops=lib.TopBooks(s,e,topN);
				if(Tops.empty()){
					cout<<"ָ��ʱ�����û�н����¼"<<endl;
				}else{
					system("cls");
					cout << "\n===== " << topN << "���������鼮 =====" << endl;
					cout << "���� | ���Ĵ��� | �鼮ID | ����" << endl;
					cout << "--------------------------------" << endl;
					for (unsigned int i = 0; i < Tops.size(); i++) {
						Book* book = Tops[i].first;
						int count = Tops[i].second;
						if(book==nullptr){
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) << "���鼮�Ѳ�����" << endl;
						}else{
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) << book->GetId() << " | "<< book->GetName() << endl;
						}
					}
					cout << "\n���ܻ�ӭͼ��ֲ�:"<< endl;
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
			case 14:{//ͳ��һ��ʱ����������
				cout<<"����������ͳ�ƽ���";
				cout<<"����ͳ����ʼʱ�䣺"<<endl;
				Time s=InputTime();
				cout<<"����ͳ�ƽ���ʱ�䣺"<<endl;
				Time e=InputTime();
				int topN=0;
				cout<<"������ͳ�Ƶ����Ž���������(1-20)"<<endl;
				cin>>topN;
				topN=max(1,min(20,topN));
				vector<pair<User*,int>> Tops=lib.TopUsers(s,e,topN);
				if(Tops.empty()){
					cout<<"ָ��ʱ�����û�н����¼"<<endl;
				}else{
					system("cls");
					cout << "\n===== Top" << topN << "���������� =====" << endl;
					cout << "���� | ���Ĵ��� | �û�ID | �û���" << endl;
					cout << "--------------------------------" << endl;
					for (unsigned int i = 0; i < Tops.size(); i++) {
						User* user = Tops[i].first;
						int count = Tops[i].second;
						if(user==nullptr){
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) <<"�û��Ѳ�����" << endl;
						}else{
							cout << setw(2) << i+1 << "   | "<< setw(6) << count << "   | "<< setw(6) << user->GetId() << " | "<< user->GetName() << endl;
						}
					}
					cout << "\n���ܻ�ӭ���߷ֲ�:"<< endl;
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
            case 15:  // �˳�ϵͳ
				lib.Save();
                cout << "\n��лʹ�ã��ټ�!" << endl;
                return 0; 
            default:
                cout << "��Чѡ�����������!" << endl;
        }
    }
}