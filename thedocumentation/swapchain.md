# Swap Chain rules

RenderPass 1 will the main pass for geometry, so the final frame that is then fed to a compute pipeline will be set to renderPass 0
After this simply create new render passes as needed

images 0-n will be the images reserved for the final compute shader present

The vector will order the index values in this order:

RenderPass -> FrameBuffer -> Image -> VkExtent2D

Note that Images may be depth images, and the specific depth image index is for pipelines that require both.
