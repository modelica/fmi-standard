
function Export-Schema() {
	Param(
		$element, 
		$expand, 
		$preview = $false
	)
	$out = $element + "_schema.png"
	$generator = "C:\srcctrl\xsddiagram\XSDDiagramConsole\bin\Debug\XSDDiagramConsole.exe"
	& $generator -o .\docs\images\gen\$out -r $element -e $expand -d -c -z 300 -a -no-gui -y .\schema\fmi3ModelDescription.xsd
	if ($preview) {
		Start-Process .\docs\images\gen\$out
	}
}

$preview = $false

Export-Schema fmiModelDescription 1 $preview
Export-Schema TypeDefinitions 2 $preview 
Export-Schema LogCategories 3 $preview 
Export-Schema Terminals 2 $preview 
Export-Schema GraphicalRepresentation 1 $preview 
Export-Schema VendorAnnotations 2 $preview 
Export-Schema ModelVariables 2 $preview 
Export-Schema ModelStructure 2 $preview 
