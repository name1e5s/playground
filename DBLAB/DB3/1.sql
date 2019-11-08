-- 1
SELECT [学生ID号],[学生姓名] 
FROM [dbo].[student] 
WHERE [班级ID号] = 'g09402'
-- 2
SELECT [周学时],[课程学分]
FROM [dbo].[course] 
WHERE [课程名称] = '网络技术与实践'
-- 3
SELECT [学生ID号],[分数]
FROM [dbo].[student_course]
WHERE [课程ID号] = 'dep04_s001' ORDER BY [分数] DESC
-- 4
SELECT *
FROM [dbo].[student] 
WHERE [学生姓名] like '张%'
-- 5
SELECT *
FROM [dbo].[student] 
WHERE [出生时间] >= '01/01/1994' AND [出生时间] < '01/01/1996'
-- 6
SELECT [学生ID号],[学生姓名],[性别],[出生时间]
FROM [dbo].[student]
WHERE [出生时间] >= '01/01/1993' AND [性别] = '女' ORDER BY [出生时间] DESC
-- 7
SELECT *,COUNT(*) AS [人数]
FROM [dbo].[student] 
-- 8
SELECT COUNT(*) AS [人数]
FROM [dbo].[student] 
SELECT *
FROM [dbo].[student] 