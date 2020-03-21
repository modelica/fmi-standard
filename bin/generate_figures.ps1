$genDir = ".\docs\images\gen"

function Clear-Gen() {
	# Get-ChildItem $genDir\*
	Remove-Item $genDir\*
}

function Check-Existing() {
	Param(
		$file
	)
	If (Test-Path $file -PathType Leaf) {
		Write-Output "Error: File already exists."
		Exit 1
	}
}

function Export-Schema() {
	Param(
		$element, 
		$expand,
		$schema = "fmi3ModelDescription.xsd",
		$outName = $element + "_schema.png",
		[switch] $preview = $false
	)
	Write-Output "Generating schema for $element..."
	$out = "$genDir\$outName"
	Check-Existing $out
	$generator = "C:\srcctrl\xsddiagram\XSDDiagramConsole\bin\Debug\XSDDiagramConsole.exe"
	& $generator -o $out -r $element -e $expand -d -c -z 300 -a -no-gui -y .\schema\$schema
	if ($preview) {
		Start-Process $out
	}
}

Clear-Gen

Export-Schema fmiModelDescription 1 
Export-Schema UnitDefinitions 3
Export-Schema BaseUnit 2
Export-Schema DisplayUnit 2
Export-Schema TypeDefinitions 2  
Export-Schema Float64 3 -schema fmi3Type.xsd 
Export-Schema Int32 3 -schema fmi3Type.xsd
Export-Schema Boolean 3 -schema fmi3Type.xsd
Export-Schema Binary 3 -schema fmi3Type.xsd
Export-Schema Enumeration 3 -schema fmi3Type.xsd
Export-Schema Clock 3 -schema fmi3Type.xsd
Export-Schema LogCategories 3  
Export-Schema DefaultExperiment 1
Export-Schema Terminals 2  
Export-Schema TerminalMemberVariable 1
Export-Schema TerminalStreamMemberVariable 1
Export-Schema GraphicalRepresentation 2
Export-Schema VendorAnnotations 2
Export-Schema ModelVariables 2
Export-Schema fmi3VariableBase 1
Export-Schema Float64 1 -schema fmi3Variable.xsd -outName "Variable_Float64_schema.png"
Export-Schema Int32 1 -schema fmi3Variable.xsd -outName "Variable_Int32_schema.png"
Export-Schema Boolean 1 -schema fmi3Variable.xsd -outName "Variable_Boolean_schema.png"
Export-Schema Binary 1 -schema fmi3Variable.xsd -outName "Variable_Binary_schema.png"
Export-Schema Enumeration 1 -schema fmi3Variable.xsd -outName "Variable_Enumeration_schema.png"
Export-Schema Annotations 2
Export-Schema ModelStructure 2  
