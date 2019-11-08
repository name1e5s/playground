-- 8
SELECT [dbo].[student].[学生ID号], [dbo].[student].[学生姓名], [dbo].[student_course].[分数]
FROM [dbo].[student_course] INNER JOIN [dbo].[student] on [dbo].[student_course].[学生ID号] = [dbo].[student].[学生ID号]
WHERE [dbo].[student_course].[课程ID号] = 'dep04_s002' AND [dbo].[student_course].[分数] > 85
-- 9
SELECT [dbo].[student].[学生ID号], [dbo].[student].[学生姓名], [dbo].[course].[课程名称], [dbo].[student_course].[分数]
FROM [dbo].[student_course] 
INNER JOIN [dbo].[student] 
ON [dbo].[student_course].[学生ID号] = [dbo].[student].[学生ID号] 
INNER JOIN [dbo].[course]
ON [dbo].[student_course].[课程ID号] = [dbo].[course].[课程号]
-- 10
SELECT [dbo].[course].[课程名称], [dbo].[student_course].[分数], [dbo].[student_course].[学分]
FROM [dbo].[student_course] 
INNER JOIN [dbo].[student] 
ON [dbo].[student_course].[学生ID号] = [dbo].[student].[学生ID号] 
INNER JOIN [dbo].[course]
ON [dbo].[student_course].[课程ID号] = [dbo].[course].[课程号]
WHERE [dbo].[student].[学生姓名] = '林红'
-- 11
SELECT [dbo].[teacher].*, [dbo].[teacher_class].[类别名称]
FROM [dbo].[teacher] INNER JOIN [dbo].[teacher_class]
ON [dbo].[teacher].[类别编号] = [dbo].[teacher_class].[类别编号]
-- 12
SELECT * 
FROM [dbo].[teacher] FULL OUTER JOIN [dbo].[teacher_class_course]
ON [dbo].[teacher].[教师ID号] = [dbo].[teacher_class_course].[教师编号]
-- 13
SELECT [学生ID号], COUNT([课程ID号]) as CON
FROM [dbo].[student_course]
GROUP BY [学生ID号] ORDER BY CON DESC
-- 14
SELECT A.*
FROM [dbo].[teacher_class_course] AS A INNER JOIN
[dbo].[teacher] AS B on A.[教师编号] = B.[教师ID号]
INNER JOIN [dbo].[department] as C on B.[部门ID号] = C.[部门ID号]
WHERE C.[部门名称] = '计算机科学'
-- 15
SELECT 课程名称
FROM 
(SELECT A.[课程号] AS 课程号, A.[教师编号] AS 教师A, B.[教师编号] AS 教师B 
FROM [dbo].[teacher_class_course] AS A 
CROSS JOIN [dbo].[teacher_class_course] AS B
WHERE A.[课程号] = B.[课程号] AND A.[教师编号] <> B.[教师编号]) t1 
INNER JOIN [dbo].[teacher] AS A ON A.[教师ID号] = t1.[教师A] 
INNER JOIN [dbo].[teacher] AS B ON B.[教师ID号] = t1.[教师B] 
INNER JOIN [dbo].[course] AS c
ON C.[课程号] = t1.[课程号] WHERE A.[教师姓名] = '纪云' AND B.[教师姓名] = '乔红' 
-- 16
SELECT 课程名称
FROM 
(SELECT A.[课程号] AS 课程号, A.[教师编号] AS 教师A, B.[教师编号] AS 教师B 
FROM [dbo].[teacher_class_course] AS A 
CROSS JOIN [dbo].[teacher_class_course] AS B
WHERE A.[课程号] = B.[课程号]) t1 
INNER JOIN [dbo].[teacher] AS A ON A.[教师ID号] = t1.[教师A] 
INNER JOIN [dbo].[teacher] AS B ON B.[教师ID号] = t1.[教师B] 
INNER JOIN [dbo].[course] AS c
ON C.[课程号] = t1.[课程号] WHERE A.[教师姓名] = '严为' AND B.[教师姓名] <> '乔红'
-- 17
SELECT A.[学生ID号], A.[学生姓名]
FROM (SELECT [学生ID号], COUNT([课程ID号]) as CON
FROM [dbo].[student_course]
GROUP BY [学生ID号]) t1 INNER JOIN
[dbo].[student] AS A ON A.[学生ID号] = t1.[学生ID号]
WHERE t1.CON > 3
-- 18
SELECT AVG([分数]) AS [均分]
FROM [dbo].[student_course]
WHERE [课程ID号] = 'dep04_b001'
-- 19
