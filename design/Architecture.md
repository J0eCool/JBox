Architecture

Kernel
----------------
Stores db of resources
- keyed by RID (resource ID)


Resource
----------------
A unit of data
- RID for component, NULL if is a component
- binary blob of data


================

Questions

Any difference between loaded resources and cold on-disk ones?
    Consider the case of TextData, where the payload is just a string
    The interface for TextData is (modeled as) a wasm component with a get and set function
    Multiple TextData will have separate components for each one (or some other means of data isolation)
    From this we infer that non-loaded resources are just in their uninitialized state, and are otherwise not distinct


================

What next?

- variants to support SExprs
- text input in browser
- resource db storage kernel
    - backed by indexedDB? or localstorage
