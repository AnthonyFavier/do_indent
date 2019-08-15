#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int debug=0;

bool readFile(char* filename, vector<string>& text)
{
	bool ok=false;
	
	ifstream file(filename);
		
	if(file)
	{
		ok=true;
		
		for(int i=0; getline(file, text[i]); i++)
			text.push_back("");
	}
	
	else
		cout << "File opening failed" << endl;
	
	return ok;
}

void cleanText(vector<string>& text)
{
	int index_first_char=0;
	int index_last_char=0;
	string keyword;
	vector<string>::iterator it;
	
	for(int i=0; i<text.size(); i++)
	{
		// remove useless spaces
		for(index_first_char=0; index_first_char<text[i].size() && (text[i][index_first_char]=='	' || text[i][index_first_char]==' '); index_first_char++){}
		for(index_last_char=text[i].length()-1; index_last_char>0 && (text[i][index_last_char]=='	' || text[i][index_last_char]==' '); index_last_char--){}
		
		text[i].erase(0, index_first_char);
		index_last_char-=index_first_char;
		if(index_last_char<0)
			index_last_char=0;
		
		if(index_last_char!=text[i].length() && index_last_char < text[i].length()-1)
			text[i].erase(index_last_char+1, text[i].length()-1-index_last_char);
		
		// remove space after keywords
		keyword="for";
		if(text[i].find(keyword)==0 && text[i].length()>keyword.length() && text[i][keyword.length()]==' ')
			text[i].erase(text[i].begin()+keyword.length());
		keyword="if";
		if(text[i].find(keyword)==0 && text[i].length()>keyword.length() && text[i][keyword.length()]==' ')
			text[i].erase(text[i].begin()+keyword.length());
		keyword="while";
		if(text[i].find(keyword)==0 && text[i].length()>keyword.length() && text[i][keyword.length()]==' ')
			text[i].erase(text[i].begin()+keyword.length());
		
		// remove end line '{'
		if(text[i].length()>2 && text[i][text[i].length()-1]=='{')
		{
			text[i].erase(text[i].end()-1);
			it=text.begin()+i+1;
			text.insert(it, "{");
			
			for(index_last_char=text[i].length()-1; index_last_char>0 && (text[i][index_last_char]=='	' || text[i][index_last_char]==' '); index_last_char--){}
			if(index_last_char!=text[i].length() && index_last_char < text[i].length()-1)
				text[i].erase(index_last_char+1, text[i].length()-1-index_last_char);
		}
		
		// remove double space
		if(i<text.size()-1 && text[i].length()==0 && text[i+1].length()==0)
		{
			text.erase(text.begin()+i+1);
			i--;
		}
	}
	
	for(int i=0; i<text.size(); i++)
	{
		if(i<text.size()-1) // if not last line
		{
			// remove empty line afer keyword
			if((text[i]=="{"
			|| (text[i].find("case")!=string::npos && text[i][text[i].length()-1]==':')
			|| (text[i].find("public")!=string::npos && text[i][text[i].length()-1]==':')
			|| (text[i].find("protected")!=string::npos && text[i][text[i].length()-1]==':')
			|| (text[i].find("private")!=string::npos && text[i][text[i].length()-1]==':'))
			&& text[i+1].length()==0)
				text.erase(text.begin()+i+1);
			
			// remove empty line before '{'
			if(text[i].length()==0 && text[i+1]=="{")
			{
				text.erase(text.begin()+i);
				i--;
			}
			
			// remove empty line before '}'
			if(text[i].length()==0 && text[i+1]=="}")
			{
				text.erase(text.begin()+i);
				i--;
			}
		}
	}
}

void addIndent(vector<string>& text)
{
	int current_indent=0;
	int parenthesis=0;
	bool one_line=false;
	bool in_case=false;
	bool in_ap1=false;
	bool in_ap2=false;
	vector<string> new_text;
	
	for(int i=0; i<text.size(); i++)
	{
		if(debug)
			cout << "text=" << text[i] << endl;
		new_text.push_back("");
		
		// add current_indent times '	' before
		for(int j=0; j<current_indent; j++)
			new_text[i].append("	");
		new_text[i].append(text[i]);
		
		// Update current_indent
		if(i<text.size()-1) // if not last line
		{
			if(text[i]=="{"
			|| (text[i].find("public")!=string::npos && text[i][text[i].length()-1]==':')
			|| (text[i].find("protected")!=string::npos && text[i][text[i].length()-1]==':')
			|| (text[i].find("private")!=string::npos && text[i][text[i].length()-1]==':'))
			{
				if(debug)
					cout << "1" << endl;
				current_indent++;
			}
			
			if(text[i].find("case")!=string::npos && text[i][text[i].length()-1]==':')
			{
				if(debug)
					cout << "2" << endl;
				in_case=true;
				current_indent++;
			}
			
			if(in_case && text[i]=="break;")
			{
				if(debug)
					cout << "3" << endl;
				current_indent--;
			}
			
			if(text[i+1].length()>0
			&& text[i+1][0]=='}'
			|| (text[i+1].find("public")!=string::npos && text[i+1][text[i+1].length()-1]==':')
			|| (text[i+1].find("protected")!=string::npos && text[i+1][text[i+1].length()-1]==':')
			|| (text[i+1].find("private")!=string::npos && text[i+1][text[i+1].length()-1]==':'))
			{
				if(debug)
					cout << "4" << endl;
				current_indent--;
			}
			
			if(one_line)
			{
				if(debug)
					cout << "5" << endl;
				current_indent--;
				one_line=false;
			}
			
			if(parenthesis || (text[i].find("for")==0 || text[i].find("if")==0 || text[i].find("else")==0))
			{
				for(int j=0; j<text[i].length(); j++)
				{
					if(text[i][j]==34) // "
						in_ap1=!in_ap1;
					if(text[i][j]==39) // '
						in_ap2=!in_ap2;
					
					if(in_ap1==false && in_ap2==false)
					{
						if(text[i][j]=='(')
							parenthesis++;
						else if(text[i][j]==')')
							parenthesis--;
					}
				}
				
				if(parenthesis==0 && i<text.size()-1 && text[i+1]!="{")
				{
					if(text[i].find("{")==string::npos || text[i].find("}")==string::npos)
					{
						if(debug)
							cout << "6" << endl;
						current_indent++;
						one_line=true;
					}
				}
			}
		}
		
		if(current_indent<0)
		{
			cout << "ERROR INDENT<0 = " << current_indent << endl;
			current_indent=0;
			break;
		}
	}
	
	text=new_text;
}

bool writeFile(char* filename, vector<string>& text)
{
	bool ok=false;
	
	ofstream file(filename);
	
	if(file)
	{
		ok=true;
		
		for(int i=0; i<text.size(); i++)
			file << text[i] << endl;
	}
	
	else
		cout << "Writing file failed" << endl;
	
	return ok;
}

void printText(vector<string>& text)
{
	for(int i=0; i<text.size(); i++)
		cout << text[i] << endl;
}

int main(int argc, char** argv)
{
	if(argc==2 || argc==3)
	{
		if(argc==3)
		{
			if(argv[2][0]!='0')
				debug=1;
		}
		
		vector<string> text(1);
		
		if(debug)
		{
			cout << endl << "-----------------------------------------------------------------------------------------" << endl;
			cout << "readFile" << endl;
			cout << "-----------------------------------------------------------------------------------------" << endl;
		}
		
		if(readFile(argv[1], text))
		{
			if(debug)
			{
				cout << endl << "-----------------------------------------------------------------------------------------" << endl;
				cout << "cleanText" << endl;
				cout << "-----------------------------------------------------------------------------------------" << endl;
			}
			
			cleanText(text);
			
			if(debug)
			{
				printText(text);
				
				cout << endl << "-----------------------------------------------------------------------------------------" << endl;
				cout << "addIndent" << endl;
				cout << "-----------------------------------------------------------------------------------------" << endl;
			}
			
			addIndent(text);
			
			if(debug)
			{
				printText(text);
				
				cout << endl << "-----------------------------------------------------------------------------------------" << endl;
				cout << "writeFile" << endl;
				cout << "-----------------------------------------------------------------------------------------" << endl;
			}
			
			writeFile(argv[1], text);
		}
	}
	
	else
		cout << "Input argument error" << endl;
	
	return 0;
}

