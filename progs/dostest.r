/*
 * Test file for "dos.r" library
 * Vassilis Vlachoudis 1998
 */
call load "dos.r"

parse arg target
if target = "" then target = "*.*"

say getcurdir('F')
say getcurdisk()
say dosversion()
say drivespace()
say machinename()
say fileattr("DOS.R","")
/* say chdir('..')
  say selectdrive('F') */
call setdta
file = FindFirst(target,"RAHDS")
do while file <> "END"
        say file
        file = FindNext()
end
exit 1
