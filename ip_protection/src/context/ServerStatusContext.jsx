import { createContext, useContext, useEffect, useState } from "react";

const ServerStatusContext = createContext();

export function ServerStatusProvider({ children }) {
  const [online, setOnline] = useState(false);

  async function checkHealth() {
    try {
      const res = await fetch("http://localhost:18080/health");
      setOnline(res.ok);
    } catch {
      setOnline(false);
    }
  }

  useEffect(() => {
    checkHealth();                     // initial check
    const id = setInterval(checkHealth, 3000); // poll every 3s
    return () => clearInterval(id);
  }, []);

  return (
    <ServerStatusContext.Provider value={{ online }}>
      {children}
    </ServerStatusContext.Provider>
  );
}

export function useServerStatus() {
  return useContext(ServerStatusContext);
}
