<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/FMI-logo-horizontal-dark.svg">
  <source media="(prefers-color-scheme: light)" srcset="docs/images/FMI-logo-horizontal-light.svg">
  <img alt="Functional Mock-up Interface" src="docs/images/FMI-logo-horizontal-light.svg">
</picture>

# Functional Mock-up Interface

The Functional Mock-up Interface (FMI) is a free standard that defines an interface to exchange dynamic models using a combination of XML files, binaries and C code.
It's supported by [240+ tools](https://fmi-standard.org/tools/) and maintained as a [Modelica Association Project](https://modelica.org/projects).

Releases and the [latest development version](https://fmi-standard.org/docs/main/) of the specification are available on the [FMI website](https://fmi-standard.org/).

## Repository Structure

- [docs](docs) &mdash; sources of the specification document
- [headers](headers) &mdash; header files of the C-API
- [schema](schema) &mdash; XSD schema for the modelDescription.xml
- [CONTRIBUTING.md](CONTRIBUTING.md) &mdash; info on contributing to the project and building the specification document
- [README.md](README.md) &mdash; this document

## Branches

- `main` &mdash; development of the next release
- `v{2|3}.0.x` &mdash; maintenance of the 2.0 and 3.0 releases

## Layered Standards

The following [layered standards](https://github.com/modelica/fmi-standard/blob/main/docs/2_6_versioning_layered_standards.adoc#versioning-and-layered-standards) are currently under development:

- [XCP](https://github.com/modelica/fmi-ls-xcp) &mdash; information for calibration and measurement
- [Network Communication](https://github.com/modelica/fmi-ls-bus) &mdash; mapping of variables to buses
- [Structure](https://github.com/modelica/fmi-ls-struct) &mdash; semantic grouping of variables
- [REF](https://github.com/modelica/fmi-ls-ref) &mdash; stimuli and reference results

## Copyright and License

Code and documentation copyright &copy; 2008-2011 the MODELISAR consortium and 2012-2025 the Modelica Association Project FMI.
Code released under the [2-Clause BSD License](https://opensource.org/licenses/BSD-2-Clause).
Docs released under [Attribution-ShareAlike 4.0 International](https://creativecommons.org/licenses/by-sa/4.0/).
