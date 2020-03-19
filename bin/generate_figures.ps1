
<# Usage of XSDDiagramConsole:

> XSDDiagramConsole.exe [-o output.svg] [-os EXTENSION] [-r RootElement[@namespace]]* [-e N] [-d] [-z N] [-f PATH,NAME,TYPE,NAMESPACE,COMMENT,SEQ,LASTCHILD,XSDTYPE] [-a] [-y] [-u USERNAME] [-p PASSWORD] [file.xsd or URL]

Options:
-o FILE
	specifies the output image. '.png','.jpg', '.svg', '.txt', '.csv' ('.emf' on Windows) are allowed.
	If not present, the GUI is shown.
-os EXTENSION
	specifies the output image is streamed through the standard
	output. EXTENSION can be: png, jpg, svg, txt, csv.
	If not present, the GUI is shown.
-r ELEMENT
	specifies the root element of the tree.
	You can put several -r options = several root elements in the tree.
-e N
	specifies the expand level (from 0 to what you want).
	Be carefull, the result image can be huge.
-d
	Display the documentation.
-z N
	specifies the zoom percentage from 10% to 1000% (only for .png image).
	Work only with the '-o', '-os png' or '-os jpg' option.
-f PATH,NAME,TYPE,NAMESPACE,COMMENT,SEQ,LASTCHILD,XSDTYPE
	specifies the fields you want to output when rendering to a txt or csv file.
-a
	outputs the attributes in text mode only (.txt and .csv).
-y
	force huge image generation without user prompt.
-u USERNAME
	specifies a username to authenticate when a xsd dependency
	(import or include) is a secured url.
-p PASSWORD
	specifies a password to authenticate when a xsd dependency
	(import or include) is a secured url.
-no-gui
	prevents the graphical interface to be opened.
#>
function Export-Schema() {
	Param(
		$element, 
		$expand, 
		$preview = $false
	)
	$out = $element + "_schema.png"
	$generator = "C:\srcctrl\xsddiagram\XSDDiagramConsole\bin\Debug\XSDDiagramConsole.exe"
	& $generator -o .\docs\images\gen\$out -r $element -e $expand -d -z 300 -a -no-gui .\schema\fmi3ModelDescription.xsd
	if ($preview) {
		Start-Process .\docs\images\gen\$out
	}
}

Export-Schema fmiModelDescription 2
Export-Schema TypeDefinitions 2
Export-Schema LogCategories 3
Export-Schema Terminals 2
Export-Schema GraphicalRepresentation 1
Export-Schema VendorAnnotations 2
Export-Schema ModelVariables 2
Export-Schema ModelStructure 2
