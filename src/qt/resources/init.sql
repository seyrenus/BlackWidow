CREATE TABLE "data" (
	"id"	INTEGER NOT NULL PRIMARY KEY UNIQUE,
	"platform"	TEXT,
	"name"	TEXT NOT NULL,
	"detail"	TEXT NOT NULL,
	"answer"	TEXT,
	"tip"	TEXT,
	"cata"	TEXT,
	"level"	TEXT,
	"submit"	TEXT,
	"passed"	TEXT,
	"rate"	TEXT,
	"language"	TEXT,
	"status"	INTEGER
);
