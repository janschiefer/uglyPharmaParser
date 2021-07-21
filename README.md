uglyPharmaParser: Generate open drug data
-----------------------------------------

German law demands that all drugs prescribed in the country in must carry an identification code (PZN or "Pharmazentralnummer") for unique identification of active ingredients, manufacturer and dosage ( SGB V § 300 ).

You think this information should be open data...but it isn't.

Actually you have to pay €€€ for registration of drugs (what is OK) AND the usage of the pharma database (what is absolutely not OK).

Remember... this information is actually REQUIRED by federal law...and an absolute MUST for any form of open source medical programs dealing with drugs or prescriptions.

So this tool generates an open drug database in the form of a structured XML file - from free public available sources.

(And this tool is a quite ugly C++ hack...but works fine).

Requirements:
1. libxml2
2. squishyXML (see other repository)
3. pdftotext tool
