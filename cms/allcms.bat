del output.txt
call zipcms

rem for S/380 testing, use this
call runcms allcms.exec output.txt all.zip brexx-exe.vmarc

rem for S/390 testing, use this
rem mvsendec encb all.zip all.dat
rem loc2ebc all.dat xfer.card
