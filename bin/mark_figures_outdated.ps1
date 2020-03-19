function Deprecate-Image() {
	Param(
		$figure,
		$preview = $false
	)
	$fig = ".\docs\images\$figure"
	& magick.exe convert $fig -background Orange -pointsize 40 label:'Outdated' +swap  -gravity Center -append $fig
	if ($preview) {
		Start-Process $fig
	}
}

$images = @(
	"fmiModelDescription_schema_2.png",
	"BaseUnit_schema.png",
	"DisplayUnit_schema.png",
	"Real_schema.png",
	"Integer_schema.png",
	"Enumeration_schema.png",
	"DefaultExperiment_schema.png",
	"fmi3Terminal_TerminalMemberVariable_Schema.png",
	"fmi3Terminal_TerminalStreamMemberVariable_Schema.png",
	"fmiModelDescription_GraphicalRepresentation_CoordinateSystem_Schema.png",
	"fmiModelDescription_GraphicalRepresentation_Icon_Schema.png",
	"fmiModelDescription_GraphicalRepresentation_Terminal_Schema.png",
	"ScalarVariable_schema.png",
	"Real_Schema_large.png",
	"Integer_Schema_large.png",
	"Boolean_Schema_large.png",
	"String_Schema_large.png",
	"Enumeration_Schema_large.png",
	"Unknown_schema.png" 
)

foreach ($i in $images) {
	Deprecate-Image $i $true
}

