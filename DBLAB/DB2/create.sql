CREATE TABLE [dbo].[book]
(
	[教材ID号] nchar(13) NOT NULL,
	[教材名] nvarchar(255) NOT NULL,
	[出版社] nvarchar(255) NOT NULL,
	[作者] nvarchar(16) NOT NULL,
	[价格] float NOT NULL
);

CREATE TABLE [dbo].[course]
(
	[课程号] nchar(10) NOT NULL,
	[课程名称] nvarchar(255) NOT NULL,
	[书号] nchar(13) NOT NULL,
	[课程总学时] int NOT NULL,
	[周学时] int NOT NULL,
	[课程学分] int NOT NULL
);

CREATE TABLE [dbo].[class]
(
	[班级号] nchar(6) NOT NULL,
	[班主任姓名] nvarchar(255) NOT NULL,
	[教室] nvarchar(255) NOT NULL,
	[系编号] nchar(6) NOT NULL
);

CREATE TABLE [dbo].[class_course]
(
	[班级号] nchar(6) NOT NULL,
	[课程号] nchar(10) NOT NULL
);

CREATE TABLE [dbo].[department]
(
	[部门ID号] nchar(6) NOT NULL,
	[部门名称] nvarchar(255) NOT NULL,
	[部门领导] nvarchar(255) NOT NULL,
	[教师数] int NOT NULL
);

CREATE TABLE [dbo].[student]
(
	[学生ID号] [nchar](8) NOT NULL,
	[学生姓名] [nvarchar](255) NOT NULL,
	[班级ID号] [nchar](6) NOT NULL,
	[性别] [nchar](2) NOT NULL,
	[出生时间] [datetime] NOT NULL,
	[入学日期] [datetime] NOT NULL,
	[家庭住址] [nvarchar](255) NOT NULL
);

CREATE TABLE [dbo].[student_course]
(
	[课程ID号] nchar(10) NOT NULL,
	[学生ID号] nchar(8) NOT NULL,
	[分数] int NOT NULL,
	[学分] int NOT NULL,
	[学期] int NOT NULL,
	[学年] nchar(9) NOT NULL
);


CREATE TABLE [dbo].[teacher]
(
	[教师ID号] nchar(9) NOT NULL,
	[教师姓名] nvarchar(255) NOT NULL,
	[性别] nchar(2) NOT NULL,
	[出生时间] datetime NOT NULL,
	[部门ID号] nchar(6) NOT NULL,
	[职称或职业] nvarchar(255) NOT NULL,
	[联系电话] nvarchar(20) NOT NULL,
	[家庭地址] nvarchar(255) NOT NULL,
	[邮政编码] nchar(6) NOT NULL,
	[类别编号] int NOT NULL
);

CREATE TABLE [dbo].[teacher_class_course]
(
	[教师编号] nchar(9) NOT NULL,
	[课程号] nchar(10) NOT NULL,
	[班级号] nchar(6) NOT NULL,
	[学期] int NOT NULL,
	[学年] nchar(9) NOT NULL,
	[排课标识] nchar(9) NOT NULL,
	[授课地点] nvarchar(255) NOT NULL,
	[教材编号] nchar(13) NOT NULL
);

CREATE TABLE [dbo].[teacher_class]
(
	[类别编号] int,
	[类别名称] nvarchar(6)
)

ALTER TABLE [dbo].[class]
ADD [备注] nvarchar(255) NOT NULL

ALTER TABLE [dbo].[class]
ALTER COLUMN [备注] int NOT NULL

ALTER TABLE [dbo].[class]
DROP COLUMN [备注]

DROP TABLE [dbo].[class]


USE [学⽣选课]
SELECT *
INTO book
FROM OPENROWSET
('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\1-book.xls', [Sheet1$]); GO
SELECT *
INTO course
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\2-course.xls', [Sheet1$]); GO
SELECT *
INTO class
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\3-class.xls', [Sheet1$]); GO
SELECT *
INTO class_course
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\4-classcourse.xls', [Sheet1$]); GO
SELECT *
INTO department
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\5-department.xls', [Sheet1$]); GO
SELECT *
INTO student
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\6-student.xls', [Sheet1$]); GO
SELECT *
INTO student_course
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\7-studentcourse.xls', [Sheet1$]); GO
SELECT *
INTO teacher
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\8-teacher.xls', [Sheet1$]); GO
SELECT *
INTO teacher_class_course
FROM OPENROWSET
('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\9-teacher-classcourse.xls', [Sheet1$]); GO
SELECT *
INTO teacher_class
FROM OPENROWSET('Microsoft.ACE.OLEDB.12.0','Excel 12.0; Database=C:\Users\15395\Desktop\Data\10-teacherclass.xls', [Sheet1$]); GO
