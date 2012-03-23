var reqdoc = "D:/dev/CIE_N&C_6_07 - dia e noite/CIE_N&C_6_07.fla";
var reqdocarr = reqdoc.split("/");
var reqname = reqdocarr[reqdocarr.length - 1];
var seldoc = null;
var docs = fl.documents;

for (doc in docs) {
	if (docs[doc].name == reqname.substr(0, reqname.length - 4)) {
		seldoc = docs[doc];
	}
}
if (!seldoc) {
	seldoc = fl.openDocument("file:///" + reqdoc);
}

if (seldoc) {
	seldoc.testMovie();
} else {
	alert("Document " + seldoc + "not found");
}
