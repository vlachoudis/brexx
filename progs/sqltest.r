call import "sqlite.r"

call sqlite

call sqlopen("test.db")

/*say "SQL=" sql("drop table test")*/
say "SQL=" sql("create table test (id integer primary key, num float, str text, bl blob)")
say "SQL=" sql("insert into test values (?,?,?,?)")
do i=1 to 10
	say "COUNT=" sqlbind()
	say sqlreset()
	say sqlbind(1, "i", 10+i)
	say sqlbind(2, "d", i*i)
	say sqlbind(3, "t", copies("Test",i))
	say sqlbind(4, "b", copies("Bingo",i))
	say sqlstep()
end

say "SQL=" sql("select * from test;")
do i=1 until sqlstep()=="DONE"
	do i=1 to sqlget()
		call write ,sqlget(i)"|"
	end
	say
end
say
say "SQLClose=" sqlclose()
