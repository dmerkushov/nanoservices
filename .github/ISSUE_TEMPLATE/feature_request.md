---
name: Feature request
about: Suggest an idea for this project
title: 'FEATURE: '
labels: enhancement
assignees: ''

---

**Problem solved by the feature**
A clear and concise description of what the problem is.

> I'm frustrated to rebuild a plenty of nanoservices by hand when developing them as parts of an application.

**Description of the proposed solution**
If you have a proposed technical solution to the problem, please describe it here.

> A nanoservice project contains a CMakeLists file and the nanoservices runtime framework monitors the state of the project directory. As soon as the contents of the directory change, the runtime rebuilds the nanoservice by running pre-specified commands (for example, `rm -rf build/; mkdir build/; cd build/; cmake ..; make`) and loads the new version.*

**Feature optionality**
If you have any idea about when the feature should be turned on or off, please describe it here.

> This feature of the SDK runtime MUST be optional (for a specific nanoservice as well as for the SDK globally).
> 
> The end-user runtime MUST NOT support this feature, it is for the nanoservices SDK only.

**Additional context**
Add any other context or screenshots about the feature request here.
