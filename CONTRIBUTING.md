# Contributing to This Project

Please take a moment to review this document in order to make the contribution process easy and effective for everyone involved.

Following these guidelines helps to communicate that you respect the time of the developers managing and developing this open source project. In return, they should reciprocate that respect in addressing your issue or assessing patches and features.

## Building the Specification Document

The FMI Specification is written in [AsciiDoc](https://asciidoc.org/), a plain-text mark-up format that can be converted to HTML, PDF and [other formats](https://asciidoctor.org/docs/convert-documents/#selecting-an-output-format).

If you haven't already done so, [clone](https://help.github.com/articles/cloning-a-repository/) the repository with:

```sh
git clone https://github.com/modelica/fmi-standard.git
git submodule update --init --recursive
```

or use your favorite Git client.

To open the repository in [Gitpod](https://www.gitpod.io/), [fork](https://github.com/modelica/fmi-standard/fork) the repository and open:

```
https://gitpod.io/#https://github.com/<your-github-user-name>/fmi-standard
```

To build the HTML document locally you have two alternatives.

### Alternative 1: Docker (recommended)

1. Install [Docker](https://www.docker.com/get-started)
2. Run the [Asciidoctor Docker Container](https://github.com/asciidoctor/docker-asciidoctor) (in the directory of the cloned repository):

   - **Linux, Mac:**
     ```sh
     docker run --rm -it -v $PWD:/documents/ asciidoctor/docker-asciidoctor asciidoctor -B /documents/ -b html5 --failure-level WARN -v docs/index.adoc
     ```
     where `$PWD` is the current directory. If the command is not called in the directory of the cloned repository, the path has to be given explicitly.

   - **Windows:**
     ```sh
     docker run --rm -it -v %cd%:/documents/ asciidoctor/docker-asciidoctor asciidoctor -B /documents/ -b html5 --failure-level WARN -v docs/index.adoc
     ```
     where `%cd%` is the current directory. If the command is not called in the directory of the cloned repository, the path has to be given explicitly.

### Alternative 2: Ruby and the AsciiDoctor gem

1. [Install Ruby](https://www.ruby-lang.org/en/downloads/)
2. Install the [AsciiDoctor Ruby gem](https://asciidoctor.org/#installation)
3. Generate the HTML document:
   ```sh
   asciidoctor -B <fmi-standard> -b html5 --failure-level WARN -v docs/index.adoc
   ```
   where `<fmi-standard>` is the path to the cloned repository.

The generated `docs/index.html` can be viewed with any modern web browser.

## Validating the Files

To validate the files, run the following Python scripts in the root of the repository:

```sh
python .circleci/lint_docs.py
python .circleci/validate_xml.py
```

## Generating the XSD Schema Figures

Some of the figures used in the FMI Specification are automatically generated from the XSD schema. To generate these figures, change into the directory where you cloned the repository and run:

```powershell
PowerShell -ExecutionPolicy Bypass -Command generate_figures.ps1
```

to generate the figures in `docs/images/schema`.

## Using the Issue Tracker

The issue tracker is the preferred channel for [Bug reports](#bug-reports), [Feature requests](#feature-requests), and submitting [Pull requests](#pull-requests), but please respect the following restrictions:

- **Do not** use the issue tracker for personal support requests (use [Stack Overflow](https://stackoverflow.com)).
- **Do not** derail or troll issues. Keep the discussion on topic and respect the opinions of others.

## Bug Reports

A bug is a _demonstrable problem_ that is caused by the code in the repository. Good bug reports are extremely helpful - thank you!

**Guidelines for bug reports:**

1. **Use the GitHub issue search** -- check if the issue has already been reported.
2. **Check if the issue has been fixed** -- try to reproduce it using the latest `master` or development branch in the repository.
3. **Isolate the problem** -- create a reduced test case or example.

A good bug report shouldn't leave others needing to chase you up for more information. Please try to be as detailed as possible in your report. What is your environment? What steps will reproduce the issue? What browser(s) and OS experience the problem? What would you expect to be the outcome? All these details will help people to fix any potential bugs.

**Example:**

```
Short and descriptive example bug report title

A summary of the issue and the platform/OS environment in which it occurs.
If suitable, include the steps required to reproduce the bug.

1. This is the first step
2. This is the second step
3. Further steps, etc.

<url> - a link to the reduced test case

Any other information you want to share that is relevant to the issue being reported.
This might include the lines of code that you have identified as causing the bug, and potential solutions (and your opinions on their merits).
```

## Feature Requests

Feature requests are welcome. But take a moment to find out whether your idea fits with the scope and aims of the project. It's up to **you** to make a strong case to convince the project's developers of the merits of this feature. Please provide as much detail and context as possible.

## Commit Messages

Please follow [the seven rules of a great Git commit message](https://chris.beams.io/posts/git-commit/) when committing your changes:

- Separate subject from body with a blank line
- Limit the subject line to 50 characters
- Capitalize the subject line
- Do not end the subject line with a period
- Use the imperative mood in the subject line
- Wrap the body at 72 characters
- Use the body to explain what and why vs. how

**For example:**

```
Summarize changes in around 50 characters or less

More detailed explanatory text, if necessary.
Wrap it to about 72 characters or so.
In some contexts, the first line is treated as the subject of the commit and the rest of the text as the body.
The blank line separating the summary from the body is critical (unless you omit the body entirely); various tools like `log`, `shortlog` and `rebase` can get confused if you run the two together.

Explain the problem that this commit is solving.
Focus on why you are making this change as opposed to how (the code explains that).
Are there side effects or other unintuitive consequences of this change?
Here's the place to explain them.

Further paragraphs come after blank lines.

 - Bullet points are okay, too.

 - Typically a hyphen or asterisk is used for the bullet, preceded by a single space, with blank lines in between, but conventions vary here.

If you use an issue tracker, put references to them at the bottom, like this:

Resolves: #123
See also: #456, #789
```

## Pull Requests

Good pull requests - patches, improvements, new features - are a fantastic help. They should remain focused in scope and avoid containing unrelated commits.

**Please ask first** before embarking on any significant pull request (e.g. implementing features, refactoring code, porting to a different language), otherwise you risk spending a lot of time working on something that the project's developers might not want to merge into the project.

Please adhere to the coding conventions used throughout a project (indentation, accurate comments, etc.) and any other requirements (such as test coverage).

**Follow this process if you'd like your work considered for inclusion in the project:**

1. [Fork](https://help.github.com/articles/fork-a-repo/) the project, clone your fork, and configure the remotes:

   ```sh
   # Clone your fork of the repo into the current directory
   git clone https://github.com/<your-username>/fmi-standard
   # Navigate to the newly cloned directory
   cd fmi-standard
   # Assign the original repo to a remote called "upstream"
   git remote add upstream https://github.com/modelica/fmi-standard
   ```

2. If you cloned a while ago, get the latest changes from upstream:

   ```sh
   git checkout master
   git pull upstream master
   ```

3. Create a new topic branch (off the main project development branch) to contain your feature, change, or fix:

   ```sh
   git checkout -b <topic-branch-name>
   ```

4. Commit your changes in logical chunks. Please adhere to the above rules when crafting [Commit messages](#commit-messages) or your code is unlikely be merged into the main project. Use Git's [interactive rebase](https://help.github.com/articles/about-git-rebase/) feature to tidy up your commits before making them public.

5. Locally merge (or rebase) the upstream development branch into your topic branch:

   ```sh
   git pull [--rebase] upstream master
   ```

6. Push your topic branch up to your fork:

   ```sh
   git push origin <topic-branch-name>
   ```

7. [Open a Pull Request](https://help.github.com/articles/about-pull-requests/) with a clear title and description.

**IMPORTANT**: By submitting a patch, you agree to allow the project owner to license your work under the same license as that used by the project.

## Branching and Versioning

We use a branching scheme with _support_ branches that allows us to maintain multiple major and minor releases concurrently.

- **Main development branch `master`:**  
  Holds the latest development version. This is where the _next_ version of the standard is developed.

- **Support branches `support/v<major>{.<minor>}.x`:**  
  Upon every major release the latest version of `master` is tagged `v<major>.0`. The maintenance of this release is performed on a support branch `support/v<major>.x` starting at this tag.  
  Examples: `support/v3.x`, `support/v2.0.x`

- **Tags `v<major>.<minor>{.<patch>}{-{alpha|beta|rc}.<number>}`:**  
  Releases and pre-releases are tagged on the respective branches following the [Semantic Versioning](https://semver.org/) when the API, XSD schema or file structure of the FMU archive change.  
  Examples: `v3.0-alpha.3`, `v3.0-beta.2`, `v3.0-rc.1`, `v3.0`

```
master
  |
  +--->+ branch "support/v2.0.x", tag "v2.0.1"
  |    |
  |    + tag "v2.0.2"
  |
  +<--- merge PR "fix-typo-in-fmi-spec"
  |
  + tag "v3.0-alpha.6"
  |
  +--->+ branch "support/v3.x", tag "v3.0"
  |    |
  |    +--->+ branch "support/v3.0.x", tag "v3.0.1"
  |    |    |
  |    |    + tag "v3.0.1"
  |    |
  |    + tag "v3.1"
  |    |
  .    .
  .    .
```

## Release Checklist

Follow these steps to create a (pre-)release:

- Update the FMI version in the schema, XML examples, and header files
- Download the `fmi-standard.zip` artifact from [GitHub Actions](https://github.com/modelica/fmi-standard/actions) and check for completeness
- Create a tag on `master` (e.g. `v3.0-rc.1`) and push it together with a commit to trigger the CI
- Create a new [release](https://github.com/modelica/fmi-standard/releases) on GitHub for the tag
  - Check `This a pre-release` if it's not the final release
  - Add a description of the changes
  - Add the version to `fmi-standard.zip` (e.g. `fmi-standard-3.0-rc.1.zip`) and upload it to the release

## Corporate Contributor License Agreement

All contributors have to sign the [Corporate Contributor License Agreement](https://github.com/modelica/fmi-standard.org/blob/main/static/assets/FMI_CCLA_v1.0_2016_06_21.pdf) or the [Contributor License Agreement of the Modelica Association](https://github.com/modelica/ModelicaAssociationCLA/releases/download/1.1.1/ModelicaAssociationCLA_1.1.1.pdf).
Therefore, the first step is getting your company to agree and sign one of the CLAs.
This ensures that all IP contributed to the FMI standard will be licensed to the Modelica Association (MA) which in turn will sublicense the FMI standard to tool vendors implementing it and end users using it, free of charge.

## Formatting Rules

When writing or editing the specification documents please follow the [AsciiDoc Recommended Practices](https://asciidoctor.org/docs/asciidoc-recommended-practices/), particularly:

- Use [one sentence per line](https://asciidoctor.org/docs/asciidoc-recommended-practices/#one-sentence-per-line)
- Use Atx style [section titles](https://asciidoctor.org/docs/asciidoc-recommended-practices/#section-titles)
- Use four dashes (`----`) for [delimited blocks](https://asciidoctor.org/docs/asciidoc-recommended-practices/#delimited-blocks)
- Use the asterisk (`*`) as marker for nested [lists](https://asciidoctor.org/docs/asciidoc-recommended-practices/#lists)
- Use angle brackets and backticks when citing XML elements.  
  Example:
  ```
  The internal step size can be provided by the attribute <<fixedInternalStepSize>> in element `<fmiModelDescription><CoSimulation>`.
  ```
- Headings may not contain any additional formatting.

### Source Code

Only the following strings are formatted as literals (using surrounding back ticks):

- source, markup and pseudo code (and parts thereof)
- file names and paths

Quotes must only be included if they are part of the original source or markup code. Code examples should be included from `*.c`, `*.h` or `*.xml` files that are validated during CI.

### Cross References

When creating a [cross reference](https://asciidoctor.org/docs/asciidoc-writers-guide/#cross-references) (xref) use dash-separated, all-lowercase names.  
Example:

```
## FMI Common Concepts for Model Exchange and Co-Simulation [[fmi-common-concepts]]

...

These parts are defined in <<fmi-common-concepts>>.
```

To reference a function, type, definition or argument, use its name as xref and add a pre-formatted label. Do not add brackets to function names.  
Example:

```
[[fmi3SetIntervalDecimal,`fmi3SetIntervalDecimal`]]
```c
typedef fmi3Status fmi3SetIntervalDecimalTYPE(fmi3Instance instance,
                                              const fmi3ValueReference valueReferences[],
                                              size_t nValueReferences,
                                              const fmi3Float64 interval[]);
```

A Clock interval is set by the environment for the current time instant by the function <<fmi3SetIntervalDecimal>>.
```

### Spelling and Capitalization

- Names start with a capital letter.  
  Example:  
  > A Co-Simulation FMU is different from a Model Exchange FMU.

- General concepts are lower case.  
  Example:  
  > A co-simulation environment is different from a model exchange environment.

- Segments of C or XML are cited exactly as they appear in the code.  
  Example:  
  > The <<fmiModelDescription>> of a Model Exchange FMU must contain a `<ModelExchange>` element.

- Headings are to be capitalized with the following rules:
  - Capitalize the first and last word of the title or heading.
  - All other words are capitalized unless they are conjunctions (and, or, but, nor, yet, so, for), articles (a, an, the), or prepositions (in, to, of, at, by, up, for, off, on).

### Non-ASCII Characters

To avoid encoding problems AsciiDoc files may only contain ASCII characters. Non-ASCII characters can be escaped using the decimal representation of the Unicode character.  
Example:

```
`&#176;F` is not a SI unit.
```

will be rendered as

> `&#176;F` is not a SI unit.

For mathematical characters and operators, use `latexmath` commands, e.g. `pass:[latexmath:[\neq]]`.

### Font Styles

To improve readability, text should not be formatted using font styles (e.g. bold, italic or underline) with two exceptions:

- cited code elements like types, functions, variables and values are formatted as code
- states are formatted as bold text and are always references to their description, e.g. <<InitializationMode>>.

Example:

```
The function <<fmi3DoStep>> may only be called in <<StepMode>>.
```

will be rendered as

> The function <<fmi3DoStep>> may only be called in <<StepMode>>.

### Non-normative Notes

Short non-normative notes should be written as a single line prefixed with `NOTE: `, for example:

```
NOTE: An importer has to determine the outer bounding box enclosing all graphical items.
```

Longer non-normative notes may use an Asciidoctor note block:

```
[NOTE]
====
This is a longer non-normative note that spans
multiple lines and explains additional guidance.
====
```

## Adding and Editing Figures

The figures in the document should be provided as SVGs (Scalable Vector Graphics) and stored in `docs/images`. We use [draw.io](https://www.draw.io/) to create and edit the figures. When you have created or edited a figure:

- select `File > Export as > SVG...`
- check `Transparent Background`
- uncheck `Include a copy of my diagram`

to export the SVG that can be embedded into the AsciiDoc document. Make sure you also save the original file using `File > Save as...` with the same name as the SVG (file extension `.xml`) and commit the files together.
