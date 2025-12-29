import { useEffect, useState } from "react";

export default function Dashboard() {
  const [status, setStatus] = useState("checking");
  const [count, setCount] = useState(0);

  useEffect(() => {
    fetch("http://localhost:18080/health")
      .then(r => r.json())
      .then(d => setStatus(d.status))
      .catch(() => setStatus("offline"));

    fetch("http://localhost:18080/chain")
      .then(r => r.json())
      .then(d => setCount(d.length));
  }, []);

  return (
    <div>
      <h1>Dashboard</h1>
      <p>Server: {status}</p>
      <p>Total Blocks: {count}</p>
    </div>
  );
}
