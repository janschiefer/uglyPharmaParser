//============================================================================
// Name        : uglyPharmaParser.cpp
// Author      : Jan Schiefer
// Version     : 0.1
// Copyright   : GNU Lesser General Public License (LGPL)
// Description : An ugly, ugly hack to parse data from
// 				 https://www.dimdi.de/dynamic/de/arzneimittel/downloads/?dir=/festbetraege/
//				 to extract drug data using squishyXML.
//
//
// Usage:
// 1. Download PDF from https://www.dimdi.de/dynamic/de/arzneimittel/downloads/?dir=/festbetraege/
// 2. Run pdftotext -layout -nopgbrk -nodiag -x 0 -y 0 -W 305 -H 2500 [PDF FILE] festbetraege.txt
// 3. Run this tool to generate XML.
//
// Extract:
// Pharma Names
// PZNs
// Package size
// (more to come in fuure releases)
//============================================================================

#include <iostream>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <squishyXML.hpp>

using namespace std;

std::string trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t")
{
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}



int main() {

	std::ifstream myfile ("/home/jschiefer/Dokumente/festbetraege.txt");

	squishyXMLParser parser;

	squishyXMLContext myContext;

	squishyXMLDocument doc ( "1.0" );

	 if (myfile.is_open())	{

		std::string line, test_line, packungsgroesse, PZN, pharma_name;

	    std::size_t pos1, pos2;

	    unsigned long int nParsed = 0;

		bool mark = false;

		squishyXMLNode rootNode( NULL, "medicationDatabase");

		while ( getline (myfile,line) )	{

			test_line = reduce(line);

			if(test_line.empty() ) continue;

			if(( test_line.find("Arzneimittelname PZN") ) != std::string::npos) {
				mark = true;
				//cout << "PANGE BEGI" << endl;
				continue;

			}
			if(( test_line.find("GKV-Spitzenverband") ) != std::string::npos) {
				mark = false;
				//cout << "PAGE END" << endl;
				continue;
			}

			if(mark) {

				std::reverse(test_line.begin(), test_line.end());

			    pos1 = test_line.find(" ");
			    pos2 = test_line.find(" ", pos1 + 1);

			    packungsgroesse = trim( test_line.substr(0, pos1) );
     			PZN = trim( test_line.substr(pos1 + 1, 8) );
     			pharma_name = trim ( test_line.substr(pos2 + 1) );

 			   std::reverse(packungsgroesse.begin(), packungsgroesse.end());
 			   std::reverse(PZN.begin(), PZN.end());
 			   std::reverse(pharma_name.begin(), pharma_name.end());

 			   squishyXMLNode medicationNode(NULL, "medication");

 			   squishyXMLNode nameNode(NULL, "name");

 			   squishyXMLNode PZNNode(NULL, "PZN");

 			   squishyXMLNode packSizeNode(NULL, "packSize");

 		 		nameNode.setNodeContent(pharma_name);
 		 		PZNNode.setNodeContent(PZN);
 		 		packSizeNode.setNodeContent(packungsgroesse);

 		 		medicationNode.addChildNode(nameNode);
 		 		medicationNode.addChildNode(PZNNode);
 		 		medicationNode.addChildNode(packSizeNode);

 		 		rootNode.addChildNode(medicationNode);

 		 		nParsed++;

			}

		}

		myfile.close();

		doc.setRootElement(rootNode);


		doc.printDocToFile("pzn-database.xml", "UTF-8", true, true );

		cout << "Parsed " << nParsed << " elements from PDF file to XML." << endl;

	 }
	 else cout << "Unable to open file";

	return 0;
}
