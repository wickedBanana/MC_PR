#ifndef I2C_H
#define I2C_H


class i2c
{
public:
    i2c();
    i2c(char adress, const char * filePath);
    ~i2c();
    bool connect(void);
    bool send_msg(char msg[], int size);
    bool get_msg(char buffer[], int size);
    void disconnect(void);


private:
    int m_adress;
    int m_fd;
    bool m_connected = false;
    char *m_pfilePath = nullptr;

};

#endif // I2C_H
