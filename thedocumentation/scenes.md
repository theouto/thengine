# File conventions

number of objects (if I ever have more than 2^32 objects, I have bigger issues to worry about)

# Object Loading:

Name

Object file

Material (as file)

translate

scale

rotation

Light loading

Name

vec3 colour

vec3 translation

radius && intensity

# Additional notes

I may eventually remove the number of objects from the file, as I should be able to simply read until EOF.

-y is up, and z is forward.
