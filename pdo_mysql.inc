<?php

class pdo_driver
{
    private $dbh;

    const E_MYSQL_CONNECT = 1;
    const E_MYSQL_ERROR = 2;
    const E_INPUT_DATA = 3;

    /**
     * Conneting to database
     * throw Exception
     *
     * @param string $dbhost
     * @param string $dbuser
     * @param string $dbpassword
     * @param string $dbname
     * @param int $dbport default = 3306
     */
    public function __construct(& $db_data)
    {
        $dsn = "mysql:dbname={$db_data['database']};host={$db_data['host']};port={$db_data['port']}";
        $init_query = "SET NAMES {$db_data['encoding']}"; // . $GLOBALS['db']['encoding'];

        $this->dbh = new PDO($dsn,
            $db_data['login'],
            $db_data['password'],
            array(PDO::MYSQL_ATTR_INIT_COMMAND => $init_query));
    }
    /**
     * Calling SQL procedure named $proc_name
     * @param string $proc_name
     * @param bool $has_retval
     * @return array
     */
    public function pdo_exec($proc_name, $has_retval) {
        $pattern = "call {$proc_name}( ";
        if($has_retval) $pattern .= "@r,";

        $num_args = func_num_args();

        if($num_args > 2) {
            for($i = 2; $i < $num_args; $i++)
            {
                $pattern .= "?,";
            }
        }

        $pattern = substr($pattern, 0, -1);

        $pattern .= ")";

        $args = func_get_args();

        array_shift($args);
        array_shift($args);

        $args = array_merge(array($pattern), $args);

        return call_user_func_array(array($this, 'pdo_query'), $args);
    }
    public function pdo_query($pattern)
    {
        try {
            $sth = $this->dbh->prepare($pattern);

            $num_args = func_num_args();

            $args = func_get_args();

            if($num_args > 1) {

                array_shift($args);

            }

            if(!$sth->execute($args)) {
                $error_array = $sth->errorInfo();
                throw new Exception("Error execute query:".$error_array[2], self::E_MYSQL_ERROR);
            }
            do {
                $rows[] = $sth->fetchAll();
            } while($sth->nextRowset());

        }
        catch(Exception $e) {
            throw new DBException($e->getMessage());
        }

        return $rows;
    }
    public function pdo_get_retval()
    {
        $rows = $this->pdo_query("select @r - 0");

        if(isset($rows[0][0][0])) {
            return $rows[0][0][0];
        }
        else
        {
            return false;
        }
    }
}

?>
