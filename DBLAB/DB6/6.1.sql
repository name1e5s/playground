-- 定义存储过程，查询“学生”表中学号=g0940211的学生的姓名和已选课程门数，若选课门数在3门以上，输出“XX，已经完成了选课”；否则输出“XX，还需选课”
IF (EXISTS (SELECT *
FROM sys.objects
WHERE NAME = 'GetG0940211'))
DROP PROC GetG0940211;
GO
CREATE PROC GetG0940211
AS
BEGIN
    DECLARE @count INT;
    SELECT @count = COUNT(*)
    FROM [dbo].[student_course]
    WHERE [学生ID号]
= 'g0940211';
    DECLARE @name NVCHAR(8);
    SELECT @name = [学生姓名]
    from [dbo].[student]
    WHERE [学生ID号] = 'g0940211';
    IF @count > 3
    BEGIN
        PRINT @name + '，已经完成了选课'
    END
    ELSE
    BEGIN
        PRINT @name + '，还需选课'
    END
END;
GO
EXEC GetG0940211;

-- 定义存储过程，用于查询“学生选课”数据库中所有教师的姓名、性别、职称和所授课的课程名称
IF (EXISTS (SELECT *
FROM sys.objects
WHERE NAME = 'GetAllAboutTeacher'))
DROP PROC GetAllAboutTeacher;
GO
CREATE PROC GetAllAboutTeacher
AS
BEGIN
    SELECT T.[教师姓名], T.[性别], T.[职称或职业], S.[课程名称]
    FROM ([dbo].[teacher] AS T
        INNER JOIN [dbo].[teacher_class_course] AS Q
        ON Q.[教师编号] = T.[教师ID号])
        INNER JOIN [dbo].[course] AS S
        ON Q.[课程号] = S.[课程号];
END;
GO
EXEC GetAllAboutTeacher;

-- 修改存储过程dbo.ShowTeaCourse,用于查询“学生选课”数据库中所有副教授职称的教师的姓名、性别、职称和所授课的课程名称。
IF (EXISTS (SELECT *
FROM sys.objects
WHERE NAME = 'ShowTeaCourse'))
DROP PROC ShowTeaCourse;
GO
CREATE PROC ShowTeaCourse
AS
PRINT 'NMSL';
GO
EXEC ShowTeaCourse;
GO
ALTER PROC ShowTeaCourse
AS
BEGIN
    SELECT T.[教师姓名], T.[性别], T.[职称或职业], S.[课程名称]
    FROM ([dbo].[teacher] AS T
        INNER JOIN [dbo].[teacher_class_course] AS Q
        ON Q.[教师编号] = T.[教师ID号])
        INNER JOIN [dbo].[course] AS S
        ON Q.[课程号] = S.[课程号]
    WHERE T.[职称或职业] = '副教授';
END
GO
EXEC ShowTeaCourse;

-- 删除该存储过程
DROP PROC ShowTeaCourse;