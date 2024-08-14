# Contributing

We'd love your help in expanding the Experiment documentation! You can contribute
in the ways described below.

## Correcting Pages

If you find an error in the documentation, please open an issue or a pull request.
The documentation is built from markdown files in the `docs` directory of the
[Experiment repository](https://github.com/luttje/experiment-source).

You can quickly head to a specific page by clicking on the "Edit"-icon in the top
right corner of the page. This will take you to the markdown file in the repository.

### Page Metadata

Most information about functions are specified in a page's metadata. This metadata
is placed at the top of the markdown file and is used to generate the page into the
specified template.

Take the following example taken from [Entity:GetAttachment](../classes/Entity/GetAttachment.md):

```yaml
---
template: lua-class-function.html
title: GetAttachment
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetAttachment
  realm: shared
  description: "Get the attachment table for the specified attachment (by bone id or attachment name)"
  argumentSets:
    - arguments:
      - name: "boneId"
        type: number
    - arguments:
      - name: "attachmentName"
        type: string
  returns:
    - type: table
      description: "The attachment information"
---
```

Say that you want to correct the description of the function. You can do this by
changing the `description` field in the metadata. If you want to correct the return
type, you can change the `type` field in the `returns` section. And so on.

## Adding Pages

Functions, classes and other content is usually automatically generated from the
source code. For that reason it is best to create an issue if you feel like something
is missing.
If you want to ensure documentation is generated yourself, follow the examples in
the source-code that show how to use the `LUA_BINDING_*` macros.

## Adding Examples to Pages

In the example above you can see that the `needs-example` tag is used. This tag is
used to mark content that needs an example. You can help by providing an example in
the content of the page. The content begins after the metadata and is written in
markdown.

For a demonstration of how to add an example, see the
[Bitwise.band](../libraries/Bitwise/band.md) page which contains two examples.

??? note "Further examples of documentation"

    The page for `Bitwise.band` also shows how to add remarks to a page.

    Since all markdown pages are open-source you can also look at the source code
    of any page to see how it is written.
