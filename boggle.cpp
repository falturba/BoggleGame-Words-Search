//Author: Fawaz Alturbaq : 5/6/2017  : fawaz.cs@gmail.com

#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;
//___________________________Trie Class ______________________//
class Trie
{
private:
	struct Node
	{
		Node *letters[26];
		bool isWord;
	};

	//Recursively releasing all the trie nodes
	void clear(Node* n)
	{
		for(int i=0;i<26;i++)
		{
			if(n->letters[i]!=nullptr)
				clear(n->letters[i]);
		}
		delete n;
	}

public:
	Node* root;
	Trie()
	{
		root = new Node();
	}
	~Trie()
	{
		clear(root);
	}

	void insertWord(string word)
	{
		Node* current = root;
		int wordLength = word.length();
		for(int i = 0;i<wordLength;i++)
		{
			int location = tolower(word[i]) - 'a';
			if(current->letters[location]==nullptr)
				current->letters[location] =  new Node();
			current = current->letters[location];
		}
		current->isWord = true;
	}


	bool isWord(const string &word) const
	{
		Node* current = root;
		int wordLength = word.length();
		for(int i=0;i<wordLength;i++)
		{
			int location = tolower(word[i]) - 'a';
			if(current->letters[location]==nullptr)
			{
				return false;
			}
			else current = current->letters[location];
		}
		return current->isWord;
	}


	bool isPrefix(string word) const
	{
		Node* current = root;
		int wordLength = word.length();
		for(int i=0;i<wordLength;i++)
		{
			int location = tolower(word[i]) - 'a';
			if(current->letters[location]==nullptr)
			{
				return false;
			}
			else current = current->letters[location];
		}
		for(int i=0;i<26;i++)
		{
			if(current->letters[i] != nullptr) return true;
		}
		return false;
	}
};


//______________________________Boggle Class _________________________//
class Boggle
{
private:
	struct Track
	{
		string word; //push the possible next letter if isPrefix is true
		int row;
		int col;
		bool **selected;
		Track(int startingRowPos,int startingColPos,int boardLength):row(startingRowPos),col(startingColPos)
		{
		//allocate 2d array to avoid stepping on previous visitied letters
			selected = new bool*[boardLength];
			for(int i=0;i<boardLength;i++)
			{
				selected[i] = new bool[boardLength];
				for(int j=0;j<boardLength;j++)
				{
					selected[i][j] = false;  // set the selected values to false
				}
			}
		}
	};

	int length;  //example 4
	char **board;
	const Trie *dictionary;
	vector<string> wordsFound;
	
	void checkNieghbor(int rowShift,int colShift, Track &track,stack<Track> &searchTrack)
	{
		int row = track.row + rowShift;
		int col = track.col + colShift;

		if(row >= 0 && col >= 0 && row < length && col < length && !track.selected[row][col])
		{
			
			string wordTry = track.word + board[row][col];
			if(wordTry.size() >= 3 && dictionary->isWord(wordTry))
			{
				if(find(wordsFound.begin(),wordsFound.end(),wordTry) == wordsFound.end())
				{
					wordsFound.push_back(wordTry);
				}
			}
			if(dictionary->isPrefix(wordTry))
			{
				Track branchedTrack(row,col,length);
				for(int i=0;i<length;i++)
					for(int j=0;j<length;j++)
						branchedTrack.selected[i][j] = track.selected[i][j];
					branchedTrack.word = wordTry;
					branchedTrack.selected[row][col] = true;
					searchTrack.push(branchedTrack);
				}


			}

		}


	public:
		Boggle(const int length,const Trie &dictionary):length(length),board(new char*[length]),dictionary(&dictionary){}
		void insertline(string s, int row)
		{
			board[row] = new char[length];
			for(int j=0;j<length;j++)
			{
				board[row][j] = s[j];
			}
		}

		void searchWords()
		{
			ofstream outfile;
			outfile.open("BoggleFoundWords.txt",ofstream::out);


		for(int row=0;row<length;row++)   //start with every letter on the board
			for(int col=0;col<length;col++)
			{
				Track root(row,col,length); // start a new track
				stack<Track> searchTrack; // build a stack to do backtracking and comeback to
											//previous route to search every possible path in 
											// a new track

				root.selected[row][col] = true; //mark the current letter as selected
				
				root.word.push_back(board[row][col]); //add the letter to the tracking word
				searchTrack.push(root);  			
				while(!searchTrack.empty())        
				{
					Track track = searchTrack.top();
					searchTrack.pop();
					 checkNieghbor(-1,-1,track,searchTrack);  //check neighbor && branch a new track if it wasn't selected before
					 checkNieghbor(-1,0,track,searchTrack);	// Also copy the visitied previous track before branching
					 checkNieghbor(-1,1,track,searchTrack);
					 checkNieghbor(0,-1,track,searchTrack);
					 checkNieghbor(0,1,track,searchTrack);
					 checkNieghbor(1,-1,track,searchTrack);
					 checkNieghbor(1,0,track,searchTrack);
					 checkNieghbor(1,1,track,searchTrack);
					}
				}




				int count = wordsFound.size();
				for(int i=0; i< count ; i++)
				{
				//cout<<wordsFound[i]<<endl;
					outfile<<wordsFound[i];
					outfile<<endl;
				}
				outfile.close();
				cout<<"Words Found: "<<count<<endl;
				cout<<"File named BoggleFoundWords.txt has been generated with the words"<<endl;

			}

		};
		//----------------------------------- End of Boggle Class ----------------//

		//helper functions
		string trim(string const &s,int l)
		{
			char *trimmed = new char[l];
			int c =0;
			for(int i = 0; i<s.length();i++)
			{
				if(s[i] != ' ')
				{
					trimmed[c] = s[i];
					c++;
				}
			}
			return string(trimmed);
		}


		int main()
		{
			Trie trie;

			ifstream dict,boggleFile;
			dict.open("dictionary.txt",ifstream::in);
			boggleFile.open("boggle.dat",ifstream::in);
			if(dict.is_open())
			{
				if(boggleFile.is_open())
				{
					//Constructing the Trie dictionary
					while(!dict.eof())
					{

						string word;
						dict>>word;
						trie.insertWord(word);
					}

					//take the length of the board
					int length = 0 ;
					boggleFile >> length;
					string tmp;
					getline(boggleFile,tmp);
					Boggle boggle(length,trie);

					//Construct the boggle boards
					for(int i=0;i<length;i++)
					{
						string line;
						getline(boggleFile,line);
						boggle.insertline(trim(line,length),i);
					}

					//Search for all the words with backtracking
					boggle.searchWords();

				}

				else
				{
					cout<<"No boggle.dat file in the directory..."<<endl;
				}	

			}else
			cout<<"No dictionary dict.txt is found..."<<endl;
			return 0;
		}




