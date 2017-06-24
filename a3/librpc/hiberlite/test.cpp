#include "hiberlite.h"

#include <string>
#include <vector>
#include <iostream>
using namespace std;

// Schema setup
class RpcProcedure{
	friend class hiberlite::access;
	template<class Archive>

	// tell the database which fields we want to set
	void hibernate(Archive & ar) {
		ar & HIBERLITE_NVP(signature);
		ar & HIBERLITE_NVP(location);
	}

public:
	string procedure_name;
	vector<string> arguments;
	vector<string> argument_types;
	int timestamp;

	string ip_address;
	int port;
	string skeleton;
};

HIBERLITE_EXPORT_CLASS(RpcProcedure)

void createDB() {

	// set the database name
	hiberlite::Database db("Main.db");

	// register the rpc procedures table
	db.registerBeanClass<RpcProcedure>();

	//drop all tables beans will use
	db.dropModel();

	//create those tables again with proper schema
	db.createModel();
}

void addData() {

	const char* signatures[5]={"Stanley Marsh", "Kyle Broflovski", "Eric Theodore Cartman", "Kenneth McCormick", "Leopold Stotch"};
	const char* locations[5]={"Stanley Marsh", "Kyle Broflovski", "Eric Theodore Cartman", "Kenneth McCormick", "Leopold Stotch"};

	for(unsigned int i=0;i<5;i++) {
		RpcProcedure x;
		x.signature=signatures[i%5];
		x.location=locations[i%5];

		hiberlite::bean_ptr<RpcProcedure> p=db.copyBean(x);	//create a managed copy of the object
	}
}

void printDB() {

	hiberlite::Database db("Main.db");
	db.registerBeanClass<RpcProcedure>();

	cout << string(15,'=')+"\nreading the DB\n";

	vector< hiberlite::bean_ptr<RpcProcedure> > v=db.getAllBeans<RpcProcedure>();

	cout << "found " << v.size() << " persons in the database:\n";

	for(size_t j=0;j<v.size();j++){
		cout << "[signature=" << v[j]->signature << "\t";
		cout << "location=" << v[j]->location << "\t";
		cout << endl;
	}
}
/*

void modifyDB()
{
	hiberlite::Database db("sample.db");
	db.registerBeanClass<Person>();

	vector< hiberlite::bean_ptr<Person> > v=db.getAllBeans<Person>();
	cout << v[0]->name << " will be deleted.\n";
	v[0].destroy();
	cout << v[1]->name << " becomes 1 year older.\n\n";
	v[1]->age+=1;
}
*/
int main()
{
	createDB();
	printDB();
	/*
	modifyDB();
	printDB();
	modifyDB();
	printDB();
	*/
	return 0;
}
