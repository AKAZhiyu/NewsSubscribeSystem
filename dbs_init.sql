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
    _user_id INT NOT NULL,
    newspaper_id INT NOT NULL,
    duration INT NOT NULL,
    start_date DATE NOT NULL,
    PRIMARY KEY(_user_id, newspaper_id),
    FOREIGN KEY(_user_id) REFERENCES UserAccount(id),
    FOREIGN KEY(newspaper_id) REFERENCES Newspaper(id)
);

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
       ('user2', 'user_password2', '456 Pine St', 200.00);

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