# Security Policy

## Purpose

The GS1 Barcode Syntax Dictionary project is provided by GS1 AISBL as a permissive open source resource describing GS1 Application Identifier syntax, together with reference implementations of the associated GS1 Barcode Syntax Tests ("Linters").

This policy sets expectations for how security vulnerabilities are reported, assessed, remediated, and disclosed.

## Project Scope

This policy applies to the GS1 Barcode Syntax Dictionary, the reference C Linters and supporting code maintained by the project, official release artefacts, and the project's build and release processes where a defect or compromise could affect those artefacts.

Applications using these resources provide their own framework for parsing and applying the Syntax Dictionary. Vulnerabilities introduced solely by such framework code, a port or transliteration, third-party redistribution, or other integration should normally be reported to that project.

## Threat Model

The reference Linters may be used to process untrusted GS1 data, including within long-running or shared processes.

Inputs to the Linters are assumed to be supplied through their documented interfaces.

The Syntax Dictionary itself is declarative data intended to be parsed and applied by integrating software. Security of the mechanisms used to load, parse, select, store, or otherwise process Syntax Dictionary files is the responsibility of that software unless a defect originates in an artefact supplied by this project.

## What Is a Vulnerability

A security vulnerability is a defect in project-supplied executable code or release infrastructure that allows input supplied through a supported interface to have consequences beyond its intended operation.

Examples include unintended code execution, memory corruption or disclosure, denial of service through unbounded or grossly disproportionate resource use, unintended access to host resources, or security-relevant compromise of official build or release artefacts.

## What Is Not a Vulnerability

Correctness, interoperability, consistency, and reduced implementation risk are important goals of the project. Reports concerning these areas are welcome and will be taken seriously, but they are not security vulnerabilities unless they also have a security impact.

Such issues should normally be reported through the issue tracker, including incorrect or incomplete Syntax Dictionary rules, incorrect validation results, ordinary error handling, proportional resource use, and issues confined to tests, fuzzers, documentation, or development tooling that cannot affect official artefacts.

The following are normally the responsibility of the integrating application or another project:

- Parsing, loading, or otherwise processing Syntax Dictionary files, including handling malformed or untrusted files and deciding which files or locations may be used.
- Ports, transliterations, wrappers, or other implementations derived from the Syntax Dictionary or reference Linters.
- Use outside the documented Linter interfaces, and sanitisation, escaping, interpretation, or other downstream handling of returned results.
- Security issues caused by how an integrating application receives, transforms, stores, logs, or uses data before passing it to the Linters or after receiving results from them.

## Severity Assessment

Security reports are assessed according to practical impact, exploitability, affected deployment models and versions, available mitigations, and the feasibility of remediation.

## Supported Versions and Release Cadence

Development takes place on the main branch.

Releases are generally made in conjunction with updates to the relevant GS1 specifications or when changes to the Syntax Dictionary or Linters otherwise require them. Additional releases may be made when required, including for security fixes.

The latest release is the primary supported version. We aim to backport security fixes to releases made within the previous two years unless backporting is infeasible. Older releases should not be assumed to receive security fixes indefinitely.

Users are encouraged to reproduce suspected vulnerabilities against the latest release or current main branch where practical.

## Reporting a Vulnerability

Suspected security vulnerabilities should be reported privately using GitHub's private vulnerability reporting.

Select Report a vulnerability on the repository's Security Advisories page, available through the repository's Security tab.

A useful report should include, where possible, the affected version or build, relevant platform, a description of the impact, minimal reproduction steps or proof of concept, and relevant inputs, stack traces, or sanitizer output.

Please do not include real personal, confidential, or production data.

For non-security bugs, feature requests, and general questions, use the public issue tracker.

## Response and Remediation

The project aims to acknowledge and provide an initial response to security reports within one week.

Fix timing depends on severity, complexity, affected components, and the testing required for a safe release.

For critical vulnerabilities with a straightforward fix, the project aims to publish a corrected release within one to two weeks of the initial report. More complex issues may take longer; where practical, mitigations or workarounds may be provided first.

These are project targets intended to set reasonable expectations rather than firm deadlines.

## Investigation and Disclosure

Maintainers will assess whether a report is within scope, determine its likely impact and affected versions where practical, and identify an appropriate remediation or mitigation.

Reporters are asked to coordinate public disclosure so users and downstream distributors have a reasonable opportunity to apply a fix or mitigation. Serious vulnerabilities may remain private until one is available; lower-risk issues may be handled openly once their security implications are understood.

Reporters may be credited in a published advisory or release information unless they request anonymity.

## Known Security Issues

None at this time.
