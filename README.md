# **Portable AES Library**
this is an implemetation for the AES algorithm including both encryption and decryption, in the three modes ECB, CBC, and CTR, it supports AES128, AES192, and AES256.

# ** DEPENDENCIES **
neither SW nor HW dependencies, it is completely reusable and portable

# ** INSTALLATION **
1- clone the repository
2- include the public header in the project.

# ** USAGE **
1- edit the configuration preprocessors in the public header to suit the project need
2- create/init inastance of the `AES_t` structure.
3- use the encrypt and decrypt functions. 