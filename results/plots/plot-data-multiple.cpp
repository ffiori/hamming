#include<bits/stdc++.h>
using namespace std;

bool useless(char c){ return c!='-' and (c<'0' or c>'9');}
bool isnumber(char c){ return c=='.' or (c>='0' and c<='9');}

string input;
unordered_set<string> algorithms;
unordered_set<string> allAlgorithms;
set<string> allAlgorithmsSorted;
unordered_set<string> algorithmsMultiple;
unordered_map<string,map<int,map<int,map<int,string>>>> data;

void print(){
    cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
    for(int m=8; m<=32; m+=8){
        cout<<"%%%%%%%%%%%%%%%%%%%%%% For m="<<m<<endl;
        for(int r=10; r<=1000; r*=10){
			cout<<"%%%%%%%%%%%%%%%%%%%%%% Sets of "<<r<<" patterns.\n";
			for(auto alg:allAlgorithmsSorted) if(data.find(alg) != data.end()){
				cout<<"$"<<alg<<" << EOD\n";
				for(int k=1; k<=3; k++) if(data[alg][m][r][k]!="0.0" and data[alg][m][r][k]!=""){
					cout<<k<<" "<<data[alg][m][r][k]<<endl;
				}
				cout<<"EOD\n"<<endl;
			}
        }
    }
}

int main(){
    algorithms.insert("SA");
    algorithms.insert("TuSA");
    algorithms.insert("TwSA");
    algorithms.insert("ANS");
    algorithms.insert("ANS2");
    algorithms.insert("ANS2b");
    algorithms.insert("EF");
    algorithms.insert("EFS");
    algorithms.insert("BYP");
    algorithms.insert("BYPS");
    algorithms.insert("BYPSb");
    
    algorithmsMultiple.insert("MM");
    algorithmsMultiple.insert("FN");
    algorithmsMultiple.insert("BYN");
    algorithmsMultiple.insert("HBYN");
    algorithmsMultiple.insert("MBYPS");
    algorithmsMultiple.insert("MBYPSb");
    
    for(auto a:algorithms) { allAlgorithms.insert(a);allAlgorithmsSorted.insert(a);}
    for(auto a:algorithmsMultiple) { allAlgorithms.insert(a);allAlgorithmsSorted.insert(a);}
    
    bool printedDNA=false;
    
    string last;
    int r;
    while(cin>>input){
        if(input=="English" and !printedDNA) { print(); printedDNA=true;}
        
        if(input == "patterns") r=atoi(last.c_str());
        
        if(allAlgorithms.find(input) == allAlgorithms.end()){
			last=input;
            continue;
		}
            
		for(int m=8; m<=32; m+=8){
			for(int k=1; k<=3; k++) {
				if(m==8 and k>1)
					break;
				
				char c;
				for(cin>>c; useless(c); cin>>c);
				
				string number;
				if(c=='-') number="0.0";
				else{
					for(; isnumber(c); cin>>c)
						number.push_back(c);
				}
				data[input][m][r][k]=number;
			}
		}
    }
    
    print();        
}


        //~ for(int m=8; m<=32; m+=8){
            //~ cout<<"\n$"<<input<<" << EOD\n";
            //~ for(int k=1; k<=3; k++){
                //~ char c;
                //~ for(cin>>c; useless(c); cin>>c);
                
                //~ string number;
                //~ if(c=='-') number="0.0";
                //~ else{
                    //~ for(; isnumber(c); cin>>c)
                        //~ number.push_back(c);
                //~ }
                //~ cout<<k<<" "<<number<<endl;
            //~ }
        //~ }
