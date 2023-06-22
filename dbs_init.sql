CREATE DATABASE IF NOT EXISTS subscription_db;
USE subscription_db;

CREATE TABLE IF NOT EXISTS UserAccount (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(255) NOT NULL,
    password VARCHAR(255) NOT NULL,
    address VARCHAR(255) NOT NULL,
    balance DECIMAL(10,2) NOT NULL
);

CREATE TABLE IF NOT EXISTS Publisher (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(255) NOT NULL,
    password VARCHAR(255) NOT NULL,
    address VARCHAR(255) NOT NULL,
    introduction TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Newspaper (
    id INT AUTO_INCREMENT PRIMARY KEY,
    newspaper_name VARCHAR(255) NOT NULL,
    cover VARCHAR(255) NOT NULL,
    _period INT NOT NULL,
    introduction TEXT NOT NULL,
    price DECIMAL(10,2) NOT NULL,
    _type ENUM('Lifestyle', 'News', 'Entertainment', 'Sports', 'Economics', 'Fashion',
               'Technology', 'Travel', 'Food', 'Comics', 'Others') NOT NULL
);

CREATE TABLE IF NOT EXISTS _Admin (
    id INT AUTO_INCREMENT PRIMARY KEY,
    password VARCHAR(255) NOT NULL
);

CREATE TABLE IF NOT EXISTS Subscription (
    subscription_id INT AUTO_INCREMENT PRIMARY KEY,
    _user_id INT NOT NULL,
    newspaper_id INT NOT NULL,
    duration INT NOT NULL,
    start_date DATE NOT NULL,
    end_date DATE,
    FOREIGN KEY(_user_id) REFERENCES UserAccount(id),
    FOREIGN KEY(newspaper_id) REFERENCES Newspaper(id)
);

DELIMITER //
CREATE TRIGGER update_end_date
BEFORE UPDATE ON Subscription
FOR EACH ROW
BEGIN
    IF NEW.start_date <> OLD.start_date OR NEW.duration <> OLD.duration THEN
        SET NEW.end_date = DATE_ADD(NEW.start_date, INTERVAL NEW.duration DAY);
    END IF;
END;
//
DELIMITER ;

DELIMITER //
CREATE TRIGGER calculate_end_date
BEFORE INSERT ON Subscription
FOR EACH ROW
BEGIN
    SET NEW.end_date = DATE_ADD(NEW.start_date, INTERVAL NEW.duration DAY);
END;
//
DELIMITER ;


CREATE TABLE IF NOT EXISTS Publication (

    publisher_id INT NOT NULL,
    newspaper_id INT NOT NULL,
    publication_date DATE NOT NULL,
    PRIMARY KEY(publisher_id, newspaper_id),
    FOREIGN KEY(publisher_id) REFERENCES Publisher(id),
    FOREIGN KEY(newspaper_id) REFERENCES Newspaper(id)
);

-- 插入用户数据
INSERT INTO UserAccount (username, password, address, balance)
VALUES ('user1', 'user_password1', '123 Main St', 100.00),
       ('user2', 'user_password2', '456 Pine St', 200.00),
       ('user3', 'user_password3', '789 Oak St', 150.00),
       ('user4', 'user_password4', '321 Elm St', 300.00),
       ('user5', 'user_password5', '555 Maple St', 50.00);

-- 插入出版商数据
INSERT INTO Publisher (username, password, address, introduction)
VALUES ('Conde_Nast', 'Conde_Nast_password', 'New York, NY, USA',
        'Condé Nast is a global media company producing premium content for more than 263 million consumers in 30 markets.'),
       ('News_UK', 'News_UK_password', 'London, UK',
       'News UK is a British newspaper publisher and a wholly owned subsidiary of American newspaper conglomerate News Corp.');

-- 插入报刊数据
INSERT INTO Newspaper (newspaper_name, cover, _period, introduction, price, _type)
VALUES ('The New Yorker', '/covers/newyorker.jpg', 7, 'American magazine of reportage, commentary, criticism, essays, fiction, satire, cartoons, and poetry.', 8.99, 'Lifestyle'),
       ('The Times', '/covers/times.jpg', 1, 'British daily national newspaper based in London.', 1.50, 'News');

-- 插入管理员数据
INSERT INTO _Admin (password)
VALUES ('admin_password1'),
       ('admin_password2');

-- 插入订阅数据
INSERT INTO Subscription (_user_id, newspaper_id, duration, start_date)
VALUES (1, 1, 30, '2023-01-01'),
       (2, 2, 90, '2023-02-01');

-- 插入出版数据
INSERT INTO Publication (publisher_id, newspaper_id, publication_date)
VALUES (1, 1, '2023-01-01'),
       (2, 2, '2023-02-01');

CREATE VIEW UserSubscription AS
SELECT
    U.id AS user_id,
    U.username AS username,
    N.newspaper_name AS newspaper_name,
    N.id AS newspaper_id,
    S.start_date AS start_date,
    DATE_ADD(S.start_date, INTERVAL S.duration DAY) AS end_date
FROM
    UserAccount U
JOIN
    Subscription S ON U.id = S._user_id
JOIN
    Newspaper N ON S.newspaper_id = N.id;

CREATE VIEW PublisherNewspaper AS
SELECT
    P.id AS publisher_id,
    P.username AS publisher_name,
    N.id AS newspaper_id,
    N.newspaper_name AS newspaper_name,
    Pub.publication_date AS publication_date
FROM
    Publisher P
JOIN
    Publication Pub ON P.id = Pub.publisher_id
JOIN
    Newspaper N ON Pub.newspaper_id = N.id;

CREATE INDEX idx_user_username ON UserAccount(username);
CREATE INDEX idx_publisher_username ON Publisher(username);
CREATE INDEX idx_newspaper_name ON Newspaper(newspaper_name);
CREATE INDEX idx_newspaper_type ON Newspaper(_type);
CREATE INDEX idx_subscription_user ON Subscription(_user_id);


-- 插入更多出版商数据
INSERT INTO Publisher (username, password, address, introduction)
VALUES ('Bloomberg', 'Bloomberg_password', '731 Lexington, New York, NY, USA',
        'Bloomberg L.P. is a privately held financial, software, data, and media company headquartered in Midtown Manhattan, New York City.'),
       ('The_Economist_Group', 'The_Economist_Group_password', 'The Adelphi, 1-11 John Adam St, London, UK',
       'The Economist Group is a world renowned multinational media company headquartered in London, England. It is best known as publisher of The Economist.'),
       ('Hearst_Communications', 'Hearst_Communications_password', '300 W. 57th Street, New York, NY, USA',
       'Hearst Communications, often referred to simply as Hearst, is an American multinational mass media and business information conglomerate based in New York City, New York.'),
       ('Time_Inc', 'Time_Inc_password', '225 Liberty Street, New York, NY, USA',
       'Time Inc. was an American worldwide mass media corporation founded on November 28, 1922, by Henry Luce and Briton Hadden, and based in New York City.'),
       ('Newsweek', 'Newsweek_password', '33 Whitehall Street, New York, NY, USA',
       'Newsweek is an American weekly news magazine founded in 1933, features articles on politics, business, culture, technology and science.');

-- 插入更多报刊数据
INSERT INTO Newspaper (newspaper_name, cover, _period, introduction, price, _type)
VALUES ('Bloomberg Businessweek', '/covers/bloomberg.jpg', 7, 'Bloomberg Businessweek, known until 2010 as BusinessWeek, is a weekly business magazine.', 5.99, 'Economics'),
       ('The Economist', '/covers/economist.jpg', 7, 'The Economist is an international weekly newspaper printed in magazine-format and published digitally.', 1.99, 'Economics'),
       ('Cosmopolitan', '/covers/cosmopolitan.jpg', 30, 'Cosmopolitan is an international fashion and entertainment magazine for women.', 2.99, 'Fashion'),
       ('Time', '/covers/time.jpg', 7, 'Time is an American weekly news magazine and news website published and based in New York City.', 1.50, 'News'),
       ('Newsweek', '/covers/newsweek.jpg', 7, 'Newsweek is an American weekly news magazine founded in 1933.', 1.99, 'News');

-- 插入更多出版数据
INSERT INTO Publication (publisher_id, newspaper_id, publication_date)
VALUES (3, 3, '2023-06-01'),
       (4, 4, '2023-06-02'),
       (5, 5, '2023-06-03'),
       (6, 6, '2023-06-04'),
       (7, 7, '2023-06-05');

-- 插入更多用户数据
INSERT INTO UserAccount (username, password, address, balance)
VALUES ('user6', 'user_password6', '111 Cedar St', 350.00),
       ('user7', 'user_password7', '222 Willow St', 400.00),
       ('user8', 'user_password8', '333 Pine St', 200.00),
       ('user9', 'user_password9', '444 Maple St', 250.00),
       ('user10', 'user_password10', '555 Oak St', 300.00);

-- 插入更多用户订阅数据
INSERT INTO Subscription (_user_id, newspaper_id, duration, start_date)
VALUES (6, 6, 30, '2023-06-01'),
       (7, 7, 90, '2023-06-02'),
       (8, 1, 60, '2023-06-03'),
       (9, 2, 30, '2023-06-04'),
       (10, 3, 60, '2023-06-05'),
       (3, 5, 60, '2023-06-10'),
       (7, 2, 30, '2023-06-14'),
       (1, 7, 90, '2023-06-08'),
       (9, 4, 30, '2023-06-18'),
       (6, 3, 60, '2023-06-20'),
       (2, 1, 30, '2023-06-22'),
       (8, 6, 90, '2023-06-12'),
       (5, 2, 60, '2023-06-24'),
       (10, 1, 30, '2023-06-15'),
       (4, 7, 60, '2023-06-17');
