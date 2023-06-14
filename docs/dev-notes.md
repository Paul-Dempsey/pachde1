# Developer Notes

While it isn't really a design goal to be a very sophisticated interpreter of images, consider providing _some_ image processing to enhance the musical usefulness of the output.

- "smart blur" to smooth + enhance edges

   This could be the most direct way to improve the musical usefulness of the module

- "Posterize" the image might be another way to do the combined smoothing + edges.

- downsample large images

  Apparent speed of the traversal is dependent on image size, becuase speed is in terms of pixels/time, and the image is scaled to fit the constrained size of a module.

