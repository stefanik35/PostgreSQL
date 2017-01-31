/****************************************************************
   PROGRAM:   load.cc
   AUTHOR:    Drew Stefanik
   LOGON ID:  z1753912
   DUE DATE:  04/22/2016

   PURPOSE:   read data from an input file, process it, and
		insert the data into a postgresql database

   INPUT:     command line argument with the input file name

   OUTPUT:    standard output, standard error output

   NOTES:     none
****************************************************************/
#include <iostream>	//cout, cerr, endl
#include <cstdlib>	//exit
#include <string>	//string
#include <vector>	//vector
#include <fstream>	//ifstream
#include <sstream>	//stringstream
#include <postgresql/libpq-fe.h>

using std::cout;
using std::cerr;
using std::endl;
using std::exit;
using std::string;
using std::vector;
using std::ifstream;
using std::stringstream;

//function prototypes
void insert_SQL(PGconn *, string);
string escape_string(PGconn *, const string &);

int main(int argc, char *argv[])
{
	//exit if more than one command line argument (filename) is present or if none are present
	if(argc != 2)
	{
	        cout << "ERROR: Requires a single file name as an argument" << endl;
        	exit(-1);
	}

	//connect to the database
	PGconn * mydb;

	mydb = PQconnectdb("host=courses user=z1753912 dbname=z1753912 password=REDACTED");

	//error and quit if cannot connect to database
	if(PQstatus(mydb) != CONNECTION_OK)
	{
		cerr << "ERROR: Cannot connect to database" << endl;
		cerr << PQerrorMessage(mydb) << endl;
		exit(-1);
	}

	cout << "Connected to database" << endl;

	//declare variables
	ifstream inFile;
	string line, token, insert;
	stringstream ss;

	//open the file specified on the command line
	inFile.open(argv[1]);

	//error and quit if file fails to open
	if(inFile.fail())
	{
		cout << "ERROR: File failed to open";
		exit(-1);
	}

	//read and process the file
	if(inFile)
	{
		//get one complete line from inFile
		while(getline(inFile, line))
		{
			//put the complete line into a stringstream so it can be tokenized
			ss << line;
			vector<string> temp;

			//tokenize the string based on the delimiter
			while(getline(ss, token, '|'))
			{
				//escapes each token and adds it to a temporary vector
				temp.push_back(escape_string(mydb, token));
			}

			//process the line of data
			//each table in the database has its own insert statement (member has two)
			//each insert statement is a string built from SQL and the appropriate tokens from the temporary vector
			//each insert statement only inserts into the table if the data is not already present

			//member entity table (leader)
            //member has two insert statements, one for leaders and one for girls because the fields in the input are different
                        insert = "INSERT INTO z1753912.member(name, address, type) SELECT '"
                                + temp[4]
                                + "' ,'" + temp[5]
                                + "' ,'" + "Leader"
                                + "' WHERE NOT EXISTS (SELECT name, address FROM z1753912.member WHERE name = '"
                                + temp[4]
                                + "' and address = '" + temp[5]
                                + "');";

                        insert_SQL(mydb, insert);

			//certain fields are empty in some rows of the data
			//to avoid empty rows in the database, the insert statements for tables with potential empty fields are only used when the field is not empty
			if(temp[6].compare("") != 0)
			{
			//member entity table (girl)
                        insert = "INSERT INTO z1753912.member(name, address, type, rank) SELECT '"
                                + temp[6]
                                + "' ,'" + temp[7]
                                + "' ,'" + "Girl"
                                + "' ,'" + temp[8]
                                + "' WHERE NOT EXISTS (SELECT name, address FROM z1753912.member WHERE name = '"
                                + temp[6]
                                + "' and address = '" + temp[7]
                                + "');";

                        insert_SQL(mydb, insert);

			//customer entity table
                        insert = "INSERT INTO z1753912.customer(name, address) SELECT '"
                                + temp[9]
                                + "' ,'" + temp[10]
                                + "' WHERE NOT EXISTS (SELECT name, address FROM z1753912.customer WHERE name = '"
                                + temp[9]
								+ "' and address = '" + temp[10]
                                + "');";

                        insert_SQL(mydb, insert);
			}

			//baker entity table
                        insert = "INSERT INTO z1753912.baker(name, address) SELECT '"
                                + temp[13]
								+ "' ,'" + temp[14]
                                + "' WHERE NOT EXISTS (SELECT name FROM z1753912.baker WHERE name = '"
                                + temp[13]
								+ "');";

                        insert_SQL(mydb, insert);

			//cookie entity table
                        insert = "INSERT INTO z1753912.cookie(name) SELECT '"
                                + temp[11]
                                + "' WHERE NOT EXISTS (SELECT name FROM z1753912.cookie WHERE name = '"
                                + temp[11]
                                + "');";

                        insert_SQL(mydb, insert);

			//council entity table
						insert = "INSERT INTO z1753912.council(name, baker_name) SELECT '"
								+ temp[0]
                                + "' ,'" + temp[13]
								+ "' WHERE NOT EXISTS (SELECT name FROM z1753912.council WHERE name = '"
								+ temp[0]
								+ "');";

						insert_SQL(mydb, insert);

			//service_unit weak entity table
                        insert = "INSERT INTO z1753912.service_unit(council_name, number, name) SELECT '"
                                + temp[0]
								+ "' ,'" + temp[2]
								+ "' ,'" + temp[1]
                                + "' WHERE NOT EXISTS (SELECT council_name, number FROM z1753912.service_unit WHERE council_name = '"
                                + temp[0]
								+ "' and number = '" + temp[2]
								+ "');";

                        insert_SQL(mydb, insert);

			//troop weak entity table
                        insert = "INSERT INTO z1753912.troop(council_name, service_unit_number, number) SELECT '"
                                + temp[0]
                                + "' ,'" + temp[2]
                                + "' ,'" + temp[3]
                                + "' WHERE NOT EXISTS (SELECT council_name, service_unit_number, number FROM z1753912.troop WHERE council_name = '"
                                + temp[0]
                                + "' and service_unit_number = '" + temp[2]
                                + "' and number = '" + temp[3]
                                + "');";

                        insert_SQL(mydb, insert);

			//belong_to relationship table (leader)
                        insert = "INSERT INTO z1753912.belongs_to(council_name, service_unit_number, troop_number, member_name, member_address) SELECT '"
                                + temp[0]
                                + "' ,'" + temp[2]
                                + "' ,'" + temp[3]
                                + "' ,'" + temp[4]
                                + "' ,'" + temp[5]
                                + "' WHERE NOT EXISTS (SELECT council_name, service_unit_number, troop_number, member_name, member_address FROM z1753912.belongs_to WHERE council_name = '"
                                + temp[0]
                                + "' and service_unit_number = '" + temp[2]
                                + "' and troop_number = '" + temp[3]
                                + "' and member_name = '" + temp[4]
                                + "' and member_address = '" + temp[5]
                                + "');";

                        insert_SQL(mydb, insert);

			//offers relationship table
                        insert = "INSERT INTO z1753912.offers(baker_name, cookie_name) SELECT '"
                                + temp[13]
                                + "' ,'" + temp[11]
                                + "' WHERE NOT EXISTS (SELECT baker_name, cookie_name FROM z1753912.offers WHERE baker_name = '"
                                + temp[13]
                                + "' and cookie_name = '" + temp[11]
                                + "');";

                        insert_SQL(mydb, insert);

			//sells_for relationship table
                        insert = "INSERT INTO z1753912.sells_for(cookie_name, council_name, price) SELECT '"
                                + temp[11]
                                + "' ,'" + temp[0]
                                + "' ,'" + temp[12]
                                + "' WHERE NOT EXISTS (SELECT cookie_name FROM z1753912.sells_for WHERE cookie_name = '"
                                + temp[11]
                                + "' and council_name = '" + temp[0]
                                + "');";

                        insert_SQL(mydb, insert);

			//the empty values in the input pertain to whether a sale is individual or shop
			//if there are empty values, it is a shop sale
			//the shop sale table is only updated in this case
			if(temp[6].compare("") == 0)
			{
			//shop_sale relationship table
                        insert = "INSERT INTO z1753912.shop_sales(cookie_name, council_name, service_unit_number, troop_number, date, quantity) SELECT '"
								+ temp[11]
                                + "' ,'" + temp[0]
                                + "' ,'" + temp[2]
                                + "' ,'" + temp[3]
                                + "' ,'" + temp[16]
                                + "' ,'" + temp[15]
                                + "' WHERE NOT EXISTS (SELECT cookie_name council_name, service_unit_number, troop_number, date FROM z1753912.shop_sales WHERE cookie_name = '"
                                + temp[11]
                                + "' and council_name = '" + temp[0]
                                + "' and service_unit_number = '" + temp[2]
                                + "' and troop_number = '" + temp[3]
                                + "' and date = '" + temp[16]
                                + "');";

                        insert_SQL(mydb, insert);
			}

			//the belongs_to table and individual_sales tables are updated in the case of individual sales
            if(temp[6].compare("") != 0)
            {
			//belongs_to relationship table (girl)
                        insert = "INSERT INTO z1753912.belongs_to(council_name, service_unit_number, troop_number, member_name, member_address) SELECT '"
                                + temp[0]
                                + "' ,'" + temp[2]
                                + "' ,'" + temp[3]
                                + "' ,'" + temp[6]
                                + "' ,'" + temp[7]
                                + "' WHERE NOT EXISTS (SELECT council_name, service_unit_number, troop_number, member_name, member_address FROM z1753912.belongs_to WHERE council_name = '"
                                + temp[0]
                                + "' and service_unit_number = '" + temp[2]
                                + "' and troop_number = '" + temp[3]
                                + "' and member_name = '" + temp[6]
                                + "' and member_address = '" + temp[7]
                                + "');";

                        insert_SQL(mydb, insert);

			//individual_sales relationship table
                        insert = "INSERT INTO z1753912.individual_sales(cookie_name, customer_name, customer_address, member_name, member_address, date, quantity) SELECT '"
                                + temp[11]
                                + "' ,'" + temp[9]
                                + "' ,'" + temp[10]
                                + "' ,'" + temp[6]
                                + "' ,'" + temp[7]
                                + "' ,'" + temp[16]
                                + "' ,'" + temp[15]
                                + "' WHERE NOT EXISTS (SELECT cookie_name customer_name, customer_address, member_name, member_address, date FROM z1753912.individual_sales WHERE cookie_name = '"
                                + temp[11]
                                + "' and customer_name = '" + temp[9]
                                + "' and customer_address = '" + temp[10]
                                + "' and member_name = '" + temp[6]
								+ "' and member_address = '" + temp[7]
                                + "' and date = '" + temp[16]
                                + "');";

                        insert_SQL(mydb, insert);
			}

			//clear the stringstream for the next line of input from the file
			ss.clear();
		}
	}

	//close the file
	inFile.close();

	//close the database connection
	PQfinish(mydb);
	cout << "Database closed" << endl;

	return 0;
}

/****************************************************************
   FUNCTION:   void insert_SQL(PGconn *, string)

   ARGUMENTS:  PGconn * conn, string s

   RETURNS:    void

   NOTES:      sends the insert statement s to the database
		indicated by conn; if the statement fails
		prints an error and exits
****************************************************************/
void insert_SQL(PGconn * conn, string s)
{
	//send the insert statement to the database
	PGresult * result;
	result = PQexec(conn, s.c_str());

	//check if the insert statement was successful
	//print an error and exit if it failed
	if(PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		cerr << "ERROR: Cannot execute insert" << endl << PQresultErrorMessage(result);
		PQfinish(conn);
		cout << "Database closed" << endl;
		exit (-1);
	}
}

/****************************************************************
   FUNCTION:   string escape_string(PGconn *, const string &)

   ARGUMENTS:  PGconn * conn, const string & s

   RETURNS:    string

   NOTES:      returns an escaped string that is suitable for
		use in SQL based on the input string s
****************************************************************/
string escape_string(PGconn * conn, const string & s)
{
	//declare variables
	string result;
	char * clean;

	//allocate appropriate size to the clean string
	clean = new char [s.length() * 2 + 1];

	//call postgres function to clean the string
	PQescapeStringConn(conn, clean, s.c_str(), s.length(), 0);

	result = clean;
	delete [] clean;
	return result;
}
