# Object rendering approach
# Warning: This is mostly incoherent rambling that exists to help me organise myself

The way that image rendering will be done will be by loading model matrices into a buffer that will then be referenced by a game object via an index, similar to the current texture system. This will allow for easier exclusion of object rendering on specific pipelines, visibility culling and frustum culling.

When it's time to render an image from a pipeline the instance buffer will be updated with the relevant number of indices for objects that should be rendered, with instance data being added according to inclusion flags that may exist within the game object. Inclusion flags are chosen instead of exclusion flags due to easier debugging and saner default behaviour.
