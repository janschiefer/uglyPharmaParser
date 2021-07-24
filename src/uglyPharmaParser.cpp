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
// 2. Run in program directory: pdftotext -layout -nopgbrk -nodiag -x 0 -y 0 -W 565 -H 2500 [PDF FILE]festbetraege.txt
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
#include <string>
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

bool isNumeric( const std::string &str ) {

	bool retval = true;

	  for ( auto it = str.begin(); it!=str.end(); ++it) {
		  if( !isdigit(*it) && *it != ',' && *it != '.' && *it != '+' && *it != '-' ) retval = false;
	  }

	  return retval;
}


int main() {

	std::ifstream myfile ("festbetraege.txt");

	squishyXMLParser parser;

	squishyXMLContext myContext;

	squishyXMLDocument doc ( "1.0" );

	 if (myfile.is_open())	{

		std::string line, test_line, temp;

	    std::size_t pos1, pos2;

		std::vector<std::string> aData;

		unsigned char dataCounter;

	    unsigned long int nParsed = 0;

		bool mark = false;

		squishyXMLNode rootNode( NULL, "medicationDatabase", doc, true );

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

				pos1 = 0;
				aData.clear();

				for(dataCounter = 0; dataCounter < 9; dataCounter++) {
					pos2 = test_line.find(" ", pos1 + 1);

					temp = trim( test_line.substr(pos1, pos2 - pos1) );

					if(dataCounter == 3 && isNumeric(temp)) {
						aData.push_back("");
						dataCounter++;
					}

					std::reverse(temp.begin(), temp.end());

					aData.push_back(temp);

					pos1 = pos2;

				}

				temp = trim ( test_line.substr(pos2 + 1) );

				std::reverse(temp.begin(), temp.end());

				aData.push_back(temp);

			   squishyXMLNode medicationNode(NULL, "Medication", doc, false);


			   squishyXMLNode nameNode(NULL, "Name", doc, false);

 			   nameNode.setNodeContent(aData.at(9));

 			   medicationNode.addChildNode(nameNode);

 			   squishyXMLNode PZNNode(NULL, "PZN", doc, false);

 			   PZNNode.setNodeContent(aData.at(8));

 			   medicationNode.addChildNode(PZNNode);

 			   squishyXMLNode packSizeNode(NULL, "PackSize", doc, false);

 			   packSizeNode.setNodeContent(aData.at(7));

 			   medicationNode.addChildNode(packSizeNode);

 			  squishyXMLNode priceNode(NULL, "Price", doc, false);

 			  priceNode.setNodeContent(aData.at(6));

 			  medicationNode.addChildNode(priceNode);

 			  squishyXMLNode fixedPriceNode(NULL, "FixedPrice", doc, false);

 			  fixedPriceNode.setNodeContent(aData.at(5));

 			  medicationNode.addChildNode(fixedPriceNode);

 			  squishyXMLNode priceDiffNode(NULL, "PriceDifference", doc, false);

 			  priceDiffNode.setNodeContent(aData.at(4));

 			  medicationNode.addChildNode(priceDiffNode);

 			  squishyXMLNode actIngredientNode(NULL, "ActiveIngredient", doc, false);

 			  actIngredientNode.setNodeContent(aData.at(3));

 			  medicationNode.addChildNode(actIngredientNode);

 			 squishyXMLNode amountNode(NULL, "AmountOfActiveIngredient", doc, false);

 			 amountNode.setNodeContent(aData.at(2));

 			 medicationNode.addChildNode(amountNode);

 			 squishyXMLNode littleWNode(NULL, "PotencyEquivalent", doc, false);

 		     littleWNode.setNodeContent(aData.at(1));

 			 medicationNode.addChildNode(littleWNode);

 			 squishyXMLNode pharmaceuticalFormNode(NULL, "PharmaceuticalForm", doc, false);

 			 pharmaceuticalFormNode.setNodeContent(aData.at(0));

 			 medicationNode.addChildNode(pharmaceuticalFormNode);


 			 rootNode.addChildNode(medicationNode);

 			 nParsed++;

			}

		}

		myfile.close();

		doc.printDocToFile("pzn-database.xml", "UTF-8", true, true );

		cout << "Parsed " << nParsed << " elements from PDF file to XML." << endl;

	 }
	 else cout << "Unable to open file";

	return 0;
}
